#version 330 core

// Interpolated values from the vertex shaders
in vec3 vColor;
uniform float alpha;
 
// Ouput data
out vec4 color;

void main()
{
  color = vec4(vColor, alpha);

}