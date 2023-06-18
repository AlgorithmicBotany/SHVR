#version 330 core

layout( location = 0 ) in vec3 vertexPosition_modelspace;

uniform mat4 MVP;
uniform mat4 M;
uniform mat4 invVolume;

uniform vec3 bounds;

out vec3 vPos_VS;	//vertex position in volume space

void main()
{
    gl_Position = MVP * vec4( vertexPosition_modelspace, 1 );
	
	// Convert object from object space ([-1,1]) to volume space ([0,1])
    vPos_VS = (invVolume * M * vec4( vertexPosition_modelspace, 1 )).xyz 
		+ bounds * 0.5; 
    vPos_VS /= bounds;	
}