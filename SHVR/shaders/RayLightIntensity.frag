#version 330 core


uniform sampler3D volumeTexture;
uniform sampler3D maskTexture;
uniform sampler1D volumeColorTexture;
uniform sampler1D maskColorTexture;
uniform sampler1D aoIntensityTexture;

uniform vec3 dimensions;
uniform float UVz;
uniform float stepSize;
uniform int maxSteps;
const int NUM_RAYS = 42;
uniform vec3 rays[NUM_RAYS];
uniform vec3 cutMin;
uniform vec3 cutMax;
uniform float isoSurfaceThresh;

// Interpolated values from the vertex shaders
in vec2 UV;
 
// Ouput data
out float intensity;


vec3 getGradientCentralDiff(in vec3 pos, in vec3 offX, in vec3 offY, in vec3 offZ) {
	vec3 d = vec3(0,0,0);
	
	// CENTRAL DIFFERENCE (p = previous; n = next)
	vec3 pos_px = pos + offX;
	vec3 pos_nx = pos - offX;
	vec3 pos_py = pos + offY;
	vec3 pos_ny = pos - offY;
	vec3 pos_pz = pos + offZ;
	vec3 pos_nz = pos - offZ;
	
	// Account for if mask is transparent
	bool m_px = texture(maskColorTexture, texture(maskTexture, pos_px).x).a > 0.f;
	bool m_nx = texture(maskColorTexture, texture(maskTexture, pos_nx).x).a > 0.f;
	bool m_py = texture(maskColorTexture, texture(maskTexture, pos_py).x).a > 0.f;
	bool m_ny = texture(maskColorTexture, texture(maskTexture, pos_ny).x).a > 0.f;
	bool m_pz = texture(maskColorTexture, texture(maskTexture, pos_pz).x).a > 0.f;
	bool m_nz = texture(maskColorTexture, texture(maskTexture, pos_nz).x).a > 0.f;
	
	if(pos_px.x <= cutMax.x && m_px){d.x =  texture( volumeTexture, pos_px).x; }
	if(pos_nx.x >= cutMin.x && m_nx){d.x -= texture( volumeTexture, pos_nx).x;}
	if(pos_py.y <= cutMax.y && m_py){d.y =  texture( volumeTexture, pos_py).x; }
	if(pos_ny.y >= cutMin.y && m_ny){d.y -= texture( volumeTexture, pos_ny).x;}
	if(pos_pz.z <= cutMax.z && m_pz){d.z =  texture( volumeTexture, pos_pz).x; }
	if(pos_nz.z >= cutMin.z && m_nz){d.z -= texture( volumeTexture, pos_nz).x;}
	return d;
}

float rayCastAO(in vec3 posVS, in vec3 rayStepDirVS) {
	// Calculate distance of ray
	// *** Weight should be based on 
	float weight = 1.f / float(maxSteps);

	// Calculate the intensity of casted ray		
	float value = 0;
	float sampleIntensity = 1;
	float intensityRay = 0;
	int i = 0;
	vec3 pos = posVS;
		
	//Note: Density outside of volume is zero	
	while (i < maxSteps) {
		if(pos.x >= cutMin.x && pos.x <= cutMax.x && 
		pos.y >= cutMin.y && pos.y <= cutMax.y && 
		pos.z >= cutMin.z && pos.z <= cutMax.z) {	
			float sampleDensity = texture(volumeTexture, pos).x;
			float maskDensity = texture(maskTexture, pos).x;
			float sampleOpacity = texture(volumeColorTexture, sampleDensity).a;
			float maskOpacity = texture(maskColorTexture, maskDensity).a;
			float opacity = sampleOpacity * maskOpacity;
			
			sampleIntensity *= (1.f - opacity);
			intensityRay += sampleIntensity;
		}
		else{
			intensityRay += 1;	//No obstruction
		}
		pos += rayStepDirVS;
		i++;			
	}

	return weight * intensityRay;
}

float calculateIntensityAO() {
	vec3 posVS = vec3(UV.x, UV.y, UVz);	
	vec3 offX = vec3(1.f / dimensions.x, 0, 0);
	vec3 offY = vec3(0, 1.f / dimensions.y, 0);
	vec3 offZ = vec3(0, 0, 1.f / dimensions.z);

	int validRays = 0;	
	float lightIntensity = 0;	

	for (int i = 0; i < NUM_RAYS; i++) {
		if(dot(-getGradientCentralDiff(posVS, offX, offY, offZ), rays[i].xyz) > 0) {		
			vec3 rayStepDirVS = stepSize * rays[i].xyz;
			vec3 startPosVS = posVS + 1.5f * rayStepDirVS;
			lightIntensity += rayCastAO(startPosVS, rayStepDirVS);
			validRays++;
		}		
	}	
	
	float intensityWeight = 1 / float(validRays);
	lightIntensity = intensityWeight * lightIntensity;	// Normalize intensity with total weight
	lightIntensity = texture(aoIntensityTexture, lightIntensity).x;
	return lightIntensity;		
}


void main()
{
  intensity = calculateIntensityAO();
}