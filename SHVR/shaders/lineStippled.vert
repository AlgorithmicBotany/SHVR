//Source: https://stackoverflow.com/questions/6017176/gllinestipple-deprecated-in-opengl-3-1
#version 330 core

layout( location = 0 ) in vec3 vertexPosition_modelspace;
layout( location = 1 ) in vec3 vertexColor;

uniform mat4 MVP;
flat out vec3 startPos;	//uninterpolated position
out vec3 vertPos;		//interpolated position
out vec3 c;				//line colour

void main()
{
    vec4 pos =  MVP * vec4( vertexPosition_modelspace, 1 );
    gl_Position = pos;
	vertPos = pos.xyz / pos.w;
	startPos = vertPos;	
	c = vertexColor;
}