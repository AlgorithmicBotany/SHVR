#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 3) in vec2 uvCoordinates;

out vec2 UV;

void main()
{
    gl_Position = vec4( vertexPosition_modelspace, 1 );
    UV = uvCoordinates;
}

