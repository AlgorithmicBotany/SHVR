#version 330 core

uniform vec2 textureSize;
uniform sampler2D startingPositionTexture;

// Ouput data
out vec4 color;

void main()
{
  vec2 uv;
  uv.x = gl_FragCoord.x / textureSize.x;
  uv.y = gl_FragCoord.y / textureSize.y;

  vec4 start = texture ( startingPositionTexture, uv );
  
  color = vec4(start.xyz, 0);
}