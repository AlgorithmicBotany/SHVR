#version 330 core

layout( location = 0 ) in vec3 vertexPosition_modelspace;
layout (location = 2) in vec3 norm_modelspace;

uniform mat4 MVP;
uniform mat4 MV;
uniform vec3 inColor;

out vec3 c;
out vec3 normWorldSpace;

void main()
{
    gl_Position =  MVP * vec4( vertexPosition_modelspace, 1 );
    c = inColor;

	//multiplied by the inverse of transpose of modelView matrix to
	//be able to get light correct even after stretching the model
	normWorldSpace = normalize((MV * vec4( norm_modelspace, 0 )).xyz);
}