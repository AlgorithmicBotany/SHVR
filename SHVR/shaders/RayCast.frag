#version 400 core

in vec2 UV;

out vec4 color;

// Values that stay constant for the whole mesh.
uniform sampler3D volumeTexture;
uniform sampler3D maskTexture;
uniform sampler2D backTexture;
uniform sampler2D frontTexture;
uniform sampler1D volumeColorTexture;
uniform sampler1D maskColorTexture;

uniform float stepSize;
uniform vec3 camDirVS;
uniform vec3 lightPosVS;
uniform vec3 dimensions;

vec3 lightColor;
float phong_exponent;
float intensity_ambient;
float intensity_diffuse;
float intensity_specular;

float offX;
float offY;
float offZ;

uniform vec3 cutMax;
uniform vec3 cutMin;
uniform vec3 paintMax;
uniform vec3 paintMin;

float rand( in vec2 co )
{
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}


vec4 getSampleColor(in vec3 pos) {
	// get density value at position of casted ray
	float intensity = texture( volumeTexture, pos ).x;																				
		
	// get color value from colour map based on density value 																	  																			
	// Note: texture comes with both color(rgb) and opacity(alpha)
	return texture( volumeColorTexture, intensity );	
}

vec3 getGradientEdge(in vec3 pos) {
	vec3 d;	
	//normal of the plane is equal to the direction from the center of texture		
	if(pos.x > offX + cutMin.x && pos.x < cutMax.x - offX &&
		pos.y > offY + cutMin.y && pos.y < cutMax.y - offY &&
		pos.z > offZ + cutMin.z && pos.z < cutMax.z - offZ) {
		
		d.x = camDirVS.x;
		d.y = camDirVS.y;
		d.z = camDirVS.z;				
	}	
	else {			
		// CENTRAL DIFFERENCE (p = previous; n = next)
		float pos_px = pos.x + offX;
		float pos_nx = pos.x - offX;
		float pos_py = pos.y + offY;
		float pos_ny = pos.y - offY;
		float pos_pz = pos.z + offZ;
		float pos_nz = pos.z - offZ;
		
		if(pos_px <= cutMax.x){d.x = texture( volumeTexture, pos + vec3(offX, 0, 0) ).x; }
		if(pos_nx >= cutMin.x){d.x -=texture( volumeTexture, pos + vec3(-offX,0, 0) ).x;}
		if(pos_py <= cutMax.y){d.y = texture( volumeTexture, pos + vec3(0, offY, 0) ).x; }
		if(pos_ny >= cutMin.y){d.y -=texture( volumeTexture, pos + vec3(0,-offY, 0) ).x;}
		if(pos_pz <= cutMax.z){d.z = texture( volumeTexture, pos + vec3(0, 0, offZ) ).x; }
		if(pos_nz >= cutMin.z){d.z -=texture( volumeTexture, pos + vec3(0, 0,-offZ) ).x;}
	}
	return -d;
}

vec3 getGradientCentralDiff(in vec3 pos) {
	vec3 d = vec3(0,0,0);
	d.x = texture( volumeTexture, pos + vec3(offX, 0, 0) ).x;
	d.x -=texture( volumeTexture, pos + vec3(-offX,0, 0) ).x;
	d.y = texture( volumeTexture, pos + vec3(0, offY, 0) ).x;
	d.y -=texture( volumeTexture, pos + vec3(0,-offY, 0) ).x;
	d.z = texture( volumeTexture, pos + vec3(0, 0, offZ) ).x;
	d.z -=texture( volumeTexture, pos + vec3(0, 0,-offZ) ).x;			
	return -d;
}

vec3 getSampleLight(in vec3 pos, in vec4 dir, in vec3 normal, in vec3 sampleColor, float opacity) {
	//Calculate the light of casted ray sample
	vec3 L = normalize(lightPosVS - pos);
	vec3 V = normalize(-dir.xyz);
	vec3 H = normalize(V + L);
	
	float diffuse = max( dot(normal, L ), 0.0 );
	float specular = pow( max( dot( normal, H ), 0.0 ), phong_exponent);
	if(diffuse <= 0.) specular = 0.;
	
	// Apply light to sampled position
	
	vec3 sampleLight = vec3(0,0,0);
	sampleLight += intensity_diffuse * diffuse * sampleColor;
	sampleLight += intensity_specular * specular * lightColor;
	sampleLight += intensity_ambient * sampleColor;				  
	sampleLight *= opacity;	//scale color by opacity (necessary since color is accumulated overtime)	
	return sampleLight;
	
	return sampleColor;
}

vec4 castRay( in vec3 pos, in vec4 dir, in float stepSizeIn )
{
	float dirLen = dir.a;    
	float randStep = stepSizeIn * (0.5 + 0.5*rand( UV ) );
	pos += dir.xyz * randStep;
	float lengthTraveled = randStep;    
    vec3 stepDir = dir.xyz * stepSizeIn;
    
    vec4 accum = vec4( 0, 0, 0, 0 );
    
    float intensityMask = 0;
    vec4 colorSample;
	vec3 grad;
    
    offX = 1./dimensions.x;
    offY = 1./dimensions.y;
    offZ = 1./dimensions.z;
	
	int maxIter = 4000;
	phong_exponent = 128.0;
	
	//What are these for again?
	//intensity_ambient = 0.2;
	//intensity_diffuse = 1;
	//intensity_specular = 0.8;	
	
	intensity_ambient = 1;
	intensity_diffuse = 1;
	intensity_specular = 1;	
	lightColor = vec3(1,1,1);
	
	// Edge case (Note: Saves an "if" statement in the ray-cast loop)
	if(lengthTraveled <= dirLen) {		//Max distance				
		colorSample = getSampleColor(pos);			
		if( colorSample.a > 0.0)	
		{
			// Get sample colour and opacity 
			intensityMask = texture( maskTexture, pos ).x;
			if(intensityMask > 0.) { colorSample = texture( maskColorTexture, intensityMask); }							
			else { colorSample.a *= texture(maskColorTexture, 0).a; }						
			
			// Calculate light on sample
			grad = normalize(getGradientEdge(pos));
			colorSample.rgb = getSampleLight(pos, dir, grad, colorSample.rgb, colorSample.a);						

			// Accumulate the colour of pixel with opacity
			accum += ( 1. - accum.a ) * colorSample; 
		}			
		pos += stepDir;
		lengthTraveled += stepSizeIn;
    }

    // General case
	for( int i = 1; 						
		i < maxIter && 						//Max iteration
		lengthTraveled <= dirLen &&		//Max distance
		accum.a < 1.;					//Opacity limit is reached
		++i ) {					
		
		colorSample = getSampleColor(pos);			
		if( colorSample.a > 0.0)	
		{
			// Get sample colour and opacity 
			intensityMask = texture( maskTexture, pos ).x;
			if(intensityMask > 0.) { colorSample = texture( maskColorTexture, intensityMask); }							
			else { colorSample.a *= texture(maskColorTexture, 0).a; }						
			
			// Calculate light on sample	
			grad = normalize(getGradientCentralDiff(pos)); 
			colorSample.rgb = getSampleLight(pos, dir, grad, colorSample.rgb, colorSample.a);				

			// Accumulate the colour of pixel with opacity
			accum += ( 1. - accum.a ) * colorSample; 
		}			
		pos += stepDir;
		lengthTraveled += stepSizeIn;
    }
	
    return accum;
}


void main()
{
	vec4 dir = texture( backTexture, UV );
	vec4 pos = texture( frontTexture, UV );
	
	// Discard any initial rays that shouldn't be rendered
    if( dir.a == 0. ||					// Ray is disabled, or length is zero
		dot(vec3(dir),camDirVS) <= 0)	// Ray is going toward camera
		discard;
	
	// Render volumetric data using ray-casting
	color = castRay( pos.xyz, dir, stepSize );
   
   //Discard any rays that haven't intersected with visible voxels
   //if(color.a == 0) discard;
}
