#version 400 core

out vec4 color;

// Values that stay constant for the whole mesh.
uniform sampler3D volumeTexture;
uniform sampler2D backTexture;
uniform sampler2D frontTexture;
uniform sampler1D volumeColorTexture;

uniform float stepSize;
uniform vec2 textureSize;
uniform vec3 fillColor;
uniform vec3 outsideColor;


struct MaxMin_Volume
{
   vec3 max;
   vec3 min;
};

uniform MaxMin_Volume maxMin;

float rand( in vec2 co )
{
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}


float castRayFront( in vec3 pos, in vec4 dir, in float stepSizeIn )
{
   float dirLen = dir.a;
    
   float randStep = stepSizeIn * 0.5;
   pos += dir.xyz * randStep;
   float lengthTraveled = randStep;
    
    vec3 stepDir = dir.xyz * stepSizeIn;
	float accum = 0;
	float totalSteps = 0;
    float intensity = 0;
    vec4 colorSample;
		    
	for( int i = 0; 
		i < 4000 && 							//MAX ITERATION
		lengthTraveled <= dirLen;				//Max distance
		++i ) {

		//Overlay volume on top of cursor only if cursor is inside volume
		if( (pos.x >= maxMin.min.x && pos.x <= maxMin.max.x &&
			pos.y >= maxMin.min.y && pos.y <= maxMin.max.y &&
			pos.z >= maxMin.min.z && pos.z <= maxMin.max.z))
		{			
		
		  intensity = clamp(texture( volumeTexture, pos ).x, 0.01, 0.99);	//Obtain iso Value of 3D volumetric texture at pos
																			//Clamp to prevent werid wrapping of texture
		  																			
		  colorSample = texture( volumeColorTexture, intensity );	//Note: texture comes with both color(rgb) and opacity(alpha)
		  				
				  
		//sample only at areas where it's not transparent (ie.opacity != 0)
		  if( colorSample.a > 0.0  )	
		  {		  
			accum++;
		  }
		}
		
		pos += stepDir;
		lengthTraveled += stepSizeIn;
		totalSteps++;
    }
	
	return (accum / totalSteps);
}

void main()
{
	vec2 uv;
	uv.x = gl_FragCoord.x / textureSize.x;
	uv.y = gl_FragCoord.y / textureSize.y;

    vec4 dir = texture( backTexture, uv );
    vec4 pos = texture( frontTexture, uv );
      
    if( pos.a == 0. || dir.a == 0.)
	discard;	
	
	float fillAmount = castRayFront( pos.xyz, dir, stepSize );
		
	if(fillAmount > 0) {
		color = vec4(fillColor * fillAmount,1);
		//color = vec4(fillColor, 1);
    }
	else {
		//color = vec4(outsideColor,1);
		discard;
	}
}
