#version 420 core

uniform float depth;
 
// Ouput data
layout (depth_less) out float gl_FragDepth;

void main()
{
  gl_FragDepth = depth;
}