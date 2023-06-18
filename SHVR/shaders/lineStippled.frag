//Source: https://stackoverflow.com/questions/6017176/gllinestipple-deprecated-in-opengl-3-1
#version 330 core

flat in vec3 startPos;	//uninterpolated pos
in vec3 vertPos;		//interpolated pos
in vec3 c;

uniform vec2 textureSize;
uniform int pixelGap;

out vec4 color;

void main()
{    
    vec2  dir  = (vertPos.xy-startPos.xy) * textureSize/2.0;
    float dist = length(dir);
	
	uint bit = uint(round(dist)) & 15U;
    if (bit > uint(pixelGap)) {color = vec4(c,1);}
	else {discard;}
}