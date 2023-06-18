#version 400 core

in vec2 UV;

out vec4 color;

// Values that stay constant for the whole mesh.
uniform sampler3D volumeTexture;
uniform sampler3D maskTexture;
uniform sampler3D AOTexture;
uniform sampler2D backTexture;
uniform sampler2D frontTexture;
uniform sampler1D volumeColorTexture;
uniform sampler1D maskColorTexture;

uniform int maxSteps;
uniform float stepSize;
uniform vec3 camDirVS;
uniform vec3 lightPosVS;
uniform vec3 dimensions;
uniform float mask_t;
uniform bool maskEnabled[10];

uniform vec3 lightColor;
uniform float phong_exponent;
uniform float intensity_ambient;
uniform float intensity_diffuse;
uniform float intensity_specular;

uniform vec3 backgroundColor;
uniform float blendSize;
uniform float blendRate;

uniform float isoSurfaceThresh;
uniform vec4 isoSurfaceColor;
uniform bool isoSurfaceEnabled;

uniform vec3 cutMax;
uniform vec3 cutMin;
uniform vec3 paintMax;
uniform vec3 paintMin;

vec3 offX;
vec3 offY;
vec3 offZ;

float rand( in vec2 co )
{
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

vec4 getSampleTFColor(in vec3 pos) {		
	// get density value at position of casted ray
	float intensity = texture( volumeTexture, pos ).x;																				
		
	// get color value from colour map based on density value 																	  																			
	// Note: texture comes with both color(rgb) and opacity(alpha)
	return texture( volumeColorTexture, intensity );	
}

vec4 getSampleMaskColor(in vec3 pos, out int groupNum) {
	float maskGroupPos = texture( maskTexture, pos ).x;
	vec4 maskColor = texture( maskColorTexture, maskGroupPos);	
	groupNum = int(maskGroupPos * 255.f);		// assumes unsigned 8bit texture		

	return maskColor;
}

// Return inverse of grayscale colour, adjusted for human colour perception
vec4 getInvGrayscaleColor(in vec4 c) {
	//Transfer some shifts in values present in transfer function
	float c_invGray =  
		1 - (0.3 * c.r) + (0.59 * c.g) + (0.11 * c.b);
	return vec4(c_invGray, c_invGray, c_invGray, c.a);
}

vec4 getSampleColor(in vec4 tfColor, in vec4 maskColor, int groupNum) {
	vec4 sampleColor = tfColor;
	if(maskEnabled[groupNum]) {
		sampleColor = mask_t * maskColor + (1-mask_t) * getInvGrayscaleColor(tfColor);
	}
	
	// Apply transparency
	sampleColor.a *= maskColor.a;

	// Note: multiplying by opacity gives more precision to transparency		
	sampleColor.a *= sampleColor.a;			// Feels thinner, but more range to control opacity
	//sampleColor.a *= 2 - sampleColor.a;	//Log approximation: Nicer transition, but difficult to control opacity
	return sampleColor;
}


vec4 getSampleColor2(in vec4 tfColor, in vec4 maskColor, int groupNum, float maskRatio) {
	vec4 sampleColor = tfColor;
	if(maskEnabled[groupNum]) {
		sampleColor = mask_t * maskColor + (1-mask_t) * getInvGrayscaleColor(tfColor);
		
		// Set the colour to be the ratio
		sampleColor = maskRatio*sampleColor + (1-maskRatio)*tfColor;
	}
	
	// Apply transparency
	sampleColor.a = tfColor.a * maskColor.a;

	// Note: multiplying by opacity gives more precision to transparency		
	sampleColor.a *= sampleColor.a;			// Feels thinner, but more range to control opacity
	//sampleColor.a *= 2 - sampleColor.a;	//Log approximation: Nicer transition, but difficult to control opacity
	return sampleColor;
}


vec3 getGradientCentralDiff(in vec3 pos) {
	vec3 d = vec3(0,0,0);			
	
	// CENTRAL DIFFERENCE (p = previous; n = next)
	vec3 pos_px = pos + offX;
	vec3 pos_nx = pos - offX;
	vec3 pos_py = pos + offY;
	vec3 pos_ny = pos - offY;
	vec3 pos_pz = pos + offZ;
	vec3 pos_nz = pos - offZ;
	
	d.x =  texture( volumeTexture, pos_px ).x;
	d.x -= texture( volumeTexture, pos_nx ).x;
	d.y =  texture( volumeTexture, pos_py ).x;
	d.y -= texture( volumeTexture, pos_ny ).x;
	d.z =  texture( volumeTexture, pos_pz ).x;
	d.z -= texture( volumeTexture, pos_nz ).x;		
	return -d;
}

vec3 getGradientEdge(in vec3 pos) {
	
	//normal of the plane is equal to the direction from the center of texture		
	if(pos.x >= cutMin.x && pos.x <= cutMax.x &&
		pos.y >= cutMin.y && pos.y <= cutMax.y &&
		pos.z >= cutMin.z && pos.z <= cutMax.z) {
		vec3 d;		
		d.x = camDirVS.x;
		d.y = camDirVS.y;
		d.z = camDirVS.z;				
		return -d;
	}	
	else {			
		return getGradientCentralDiff(pos);
	}
}



vec3 getSampleLight(in vec3 pos, in vec4 dir, in vec3 normal, in vec3 cVoxel, in vec3 cLight, float opacity) {
	//Calculate the light of casted ray sample
	vec3 L = normalize(lightPosVS - pos);
	vec3 V = normalize(-dir.xyz);
	vec3 H = normalize(V + L);
	
	float ambient = texture(AOTexture, pos).x;
	float diffuse = max( dot(normal, L ), 0.0 );
	float specular = pow( max( dot( normal, H ), 0.0 ), phong_exponent);
	if(diffuse <= 0.) specular = 0.;
	
	// Apply light to sampled position		
	vec3 sampleLight = vec3(0,0,0);
	sampleLight += intensity_diffuse * diffuse * cVoxel;
	sampleLight += intensity_specular * specular * cLight;
	sampleLight += intensity_ambient * ambient * cVoxel;				  
	sampleLight *= opacity;	//scale color by opacity (necessary since color is accumulated overtime)	
	return sampleLight;
}

vec3 getSampleLightOnClippingPlane(in vec3 pos, in vec4 dir, in vec3 normal, in vec3 cVoxel, in vec3 cLight, float opacity) {
	//Calculate the light of casted ray sample
	vec3 L = normalize(lightPosVS - pos);
	vec3 V = normalize(-dir.xyz);
	vec3 H = normalize(V + L);
	
	//float ambient = texture(AOTexture, pos).x;
	float ambient = 0.5;
	float diffuse = max( dot(normal, L ), 0.0 );
	float specular = pow( max( dot( normal, H ), 0.0 ), phong_exponent);
	if(diffuse <= 0.) specular = 0.;
	
	// Apply light to sampled position		
	vec3 sampleLight = vec3(0,0,0);
	sampleLight += intensity_diffuse * diffuse * cVoxel;
	sampleLight += intensity_specular * specular * cLight;
	sampleLight += intensity_ambient * ambient * cVoxel;				  
	sampleLight *= opacity;	//scale color by opacity (necessary since color is accumulated overtime)	
	return sampleLight;
}

// Dims the colour of objects far away from viewers
vec3 distanceColorBlending(in vec3 cb, in vec3 cv, float kds, float kde, float dv) {
	// @param cb background colour
	// @param cv voxel colour
	// @param kds blend size (>0)
	// @param kde blend rate (>0)
	// @param dv distance
	// @return colour after blending

	//Note: this assumes step size is much smaller than 1
	float t = clamp(kds*pow(dv,kde),0.f,1.f);
	return (1-t)*cv + t*cb;
}

// Provides smooth falloff function (returns 1 at 0, and 0 at R)
float softObjectWyvillFunc(float r, float R) {
	//r distance
	//R max distance
    return 1.f 
        - (4.f*pow(r,6))/(9.f*pow(R,6)) 
        + (17.f*pow(r,4))/(9.f*pow(R,4)) 
        - (22.f*r*r) / (9.f*R*R);
}


vec4 castRay( in vec3 pos, in vec4 dir, in float stepSizeIn )
{
	float dirLen = dir.a;    
	float randStep = stepSizeIn * (0.5 + 0.5*rand( UV ) );
	pos += dir.xyz * randStep;
	float lengthTraveled = randStep;    
    vec3 stepDir = dir.xyz * stepSizeIn;
    
    vec4 accum = vec4( 0, 0, 0, 0 );
	float accumThickness = 0;
    
    float intensityMask = 0;
    vec4 colorSample;
	vec3 grad;
    
    offX = vec3(1./dimensions.x,0,0);
    offY = vec3(0,1./dimensions.y,0);
    offZ = vec3(0,0,1./dimensions.z);
		
	// Edge case (Note: Saves an "if" statement in the ray-cast loop)
	if(lengthTraveled <= dirLen) {		//Max distance				
		int groupNum;
		vec4 maskColor = getSampleMaskColor(pos, groupNum);	//out groupNum & color
		vec4 tfColor = getSampleTFColor(pos);						
		colorSample = getSampleColor(tfColor, maskColor, groupNum);

		if( colorSample.a > 0.0) {
			grad = normalize(getGradientEdge(pos)); 						
			colorSample.rgb = getSampleLightOnClippingPlane(pos, dir, grad, colorSample.rgb, lightColor, colorSample.a);												
			colorSample.rgb = distanceColorBlending(
				vec3(0,0,0),
				colorSample.rgb,
				blendSize,
				blendRate,
				lengthTraveled);
		
		
			// Accumulate the colour of pixel with opacity
			accum += ( 1. - accum.a ) * colorSample; 
		}			
		
		pos += stepDir;
		lengthTraveled += stepSizeIn;
    }

	bool prevIsSurface = false;
    // General case
	for( int i = 1; 						
		i < maxSteps && 					//Max iteration of steps
		lengthTraveled <= dirLen &&		//Max distance
		accum.a < 1.;					//Opacity limit is reached
		++i ) {					

		int groupNum;
		vec4 maskColor = getSampleMaskColor(pos, groupNum);	//out groupNum & color
		vec4 tfColor = getSampleTFColor(pos);				

		float intensity = texture( volumeTexture, pos ).x;
		bool isSurface = (intensity > isoSurfaceThresh);
		if(isoSurfaceEnabled &&								// Enabled seeing isoSurface
			isSurface != prevIsSurface &&						// Within the edge of surface
			groupNum == 0 &&
			pos.x > paintMin.x && pos.x < paintMax.x && 	// Within painting region
			pos.y > paintMin.y && pos.y < paintMax.y && 
			pos.z > paintMin.z && pos.z < paintMax.z) {
			float t = 0.5;
			tfColor = t*isoSurfaceColor + (1-t) * tfColor;
			maskColor = t*isoSurfaceColor + (1-t) * maskColor;
		}
		prevIsSurface = isSurface;	
		
		if( tfColor.a > 0.0)	//tf is not transparent
		{
			grad = normalize(getGradientCentralDiff(pos)); 	
			//colorSample = getSampleColor(tfColor, maskColor, groupNum);
			colorSample = getSampleColor2(tfColor, maskColor, groupNum, clamp(intensity/isoSurfaceThresh,0.f,1.f));
					
			//colorSample = vec4(1,1,1,1);
			colorSample.rgb = getSampleLight(pos, dir, grad, colorSample.rgb, lightColor, colorSample.a);				
		
			// Accumulate the colour of pixel with opacity	
			colorSample.rgb = distanceColorBlending(
				backgroundColor,
				colorSample.rgb,
				blendSize,
				blendRate,
				lengthTraveled);
			accum += ( 1. - accum.a ) * colorSample; 
			//accum += 0.05 * colorSample;
			//accumThickness += stepSizeIn;
			//accum += (1.f - softObjectWyvillFunc(accumThickness, 0.05f)) * colorSample;
			//accum += (1.f - softObjectWyvillFunc(accumThickness, 0.05f)) * colorSample;
			//accum += vec4(0.5 * colorSample.rgb, 0.5);
		}
		
		pos += stepDir;
		lengthTraveled += stepSizeIn;
    }
	
    return accum;
}



//vec4 castRay2( in vec3 pos, in vec4 dir, in float stepSizeIn )
//{
//	float dirLen = dir.a;    
//	float randStep = stepSizeIn * (0.5 + 0.5*rand( UV ) );
//	pos += dir.xyz * randStep;
//	float lengthTraveled = randStep;    
//    vec3 stepDir = dir.xyz * stepSizeIn;
//    
//    vec4 accum = vec4( 0, 0, 0, 0 );
//	float accumThickness = 0;
//    
//    float intensityMask = 0;
//    vec4 colorSample;
//	vec3 grad;
//    
//    offX = vec3(1./dimensions.x,0,0);
//    offY = vec3(0,1./dimensions.y,0);
//    offZ = vec3(0,0,1./dimensions.z);
//		
//	bool prevIsSurface = false;
//    // General case
//	for( int i = 1; 						
//		i < maxSteps && 					//Max iteration of steps
//		lengthTraveled <= dirLen &&		//Max distance
//		accum.a < 1.;					//Opacity limit is reached
//		++i ) {					
//		float ambient = texture(AOTexture, pos).x;
//		//vec4 tfColor = getSampleTFColor(pos);				
//		vec4 tfColor = ambient * vec4(1,1,1,1);
//		
//		if( tfColor.a > 0.0)	//tf is not transparent
//		{
//			colorSample = tfColor;
//			accum += ( 1. - accum.a ) * colorSample; 
//		}
//		
//		pos += stepDir;
//		lengthTraveled += stepSizeIn;
//    }
//	
//    return accum;
//}

void main()
{
	vec4 dir = texture( backTexture, UV );
	vec4 pos = texture( frontTexture, UV );
	
	// Discard any initial rays that shouldn't be rendered
    if( dir.a == 0. ||					// Ray is disabled, or length is zero
		dot(dir.xyz,camDirVS) < 0)		// Ray is going toward camera
		discard;
	
		
	// Render volumetric data using ray-casting
	color = castRay( pos.xyz, dir, stepSize );   		
	//color = castRay2( pos.xyz, dir, stepSize );   		
	

}
