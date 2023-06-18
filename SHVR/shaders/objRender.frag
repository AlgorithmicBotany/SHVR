#version 330 core

// Interpolated values from the vertex shaders
in vec3 c;
in vec3 normWorldSpace;
 
// Ouput data
out vec4 color;

void main()
{
  vec3 L = vec3(0,0,1);
  float diffuse = max( dot( normalize(normWorldSpace), L ), 0.2 );
  color = vec4(c * diffuse, 1.);
}