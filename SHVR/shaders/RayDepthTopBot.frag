#version 400 core

out vec4 color;

// Values that stay constant for the whole mesh.
uniform sampler3D volumeTexture;
uniform sampler2D backTexture;
uniform sampler2D frontTexture;
uniform sampler1D volumeColorTexture;

uniform float stepSize;
uniform vec2 textureSize;
uniform vec3 frontEdgeColor;
uniform vec3 backEdgeColor;
uniform vec3 edgeColor;
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


vec4 castRayFront( in vec3 pos, in vec4 dir, in float stepSizeIn )
{
   float dirLen = dir.a;
    
   float randStep = stepSizeIn * 0.5;
   pos += dir.xyz * randStep;
   float lengthTraveled = randStep;
    
    vec3 stepDir = dir.xyz * stepSizeIn;
	vec4 accum = vec4( 0, 0, 0, 0 );
    float intensity = 0;
    vec4 colorSample;
		    
	for( int i = 0; 
		i < 3 && 							//MAX ITERATION
			lengthTraveled <= dirLen &&			//Max distance
			accum.a < 1.;						//Opacity limit
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
			accum.a += ( 1. - accum.a ) * colorSample.a;
			return accum;
		  }
		}
		
		pos += stepDir;
		lengthTraveled += stepSizeIn;
    }
	
	return accum;
}

vec4 castRayBack( in vec3 pos, in vec4 dir, in float stepSizeIn )
{
   float dirLen = dir.a;
    
   float randStep = stepSizeIn * 0.5;
   pos += dir.xyz * randStep;
   float lengthTraveled = randStep;
    
    vec3 stepDir = dir.xyz * stepSizeIn;
    vec4 accum = vec4( 0, 0, 0, 0 );    
    float intensity = 0;
    vec4 colorSample;
	
	//Start from the back
	pos = pos + (dir.xyz*dir.a);
		    
	for( int i = 0; 
		i < 3 && 							//MAX ITERATION
			lengthTraveled <= dirLen &&			//Max distance
			accum.a < 1.;						//Opacity limit
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
			accum.a += ( 1. - accum.a ) * colorSample.a;
			return accum; 			
		  }
		}
		
		pos -= stepDir;
		lengthTraveled += stepSizeIn;
    }
	
	return accum;
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
	
	vec4 cFront = castRayFront( pos.xyz, dir, stepSize );
	vec4 cBack = castRayBack( pos.xyz, dir, stepSize );
	
	if(cFront.a > 0 && cBack.a > 0) {
		color = vec4(edgeColor,1);
    }
	else if (cFront.a > 0) {
		color = vec4(frontEdgeColor,1);
	}
	else if (cBack.a > 0) {
		color = vec4(backEdgeColor,1);
	}	
	else {
		//color = vec4(outsideColor,1);
		discard;
	}
}
