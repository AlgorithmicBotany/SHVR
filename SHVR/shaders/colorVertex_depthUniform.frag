#version 420 core

// Interpolated values from the vertex shaders
in vec3 vColor;
uniform float alpha;
uniform float depth;
 
// Ouput data
out vec4 color;
layout (depth_less) out float gl_FragDepth;

void main()
{
  color = vec4(vColor, alpha);
  gl_FragDepth = depth;
}