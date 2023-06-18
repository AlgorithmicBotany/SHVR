#version 330 core

// Interpolated values from the vertex shaders
in vec3 vPos_VS;
 
// Ouput data
out vec4 color;
uniform float rayEnabled;

void main()
{  
  color = vec4(vPos_VS, rayEnabled);
}