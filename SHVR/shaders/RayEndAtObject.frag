#version 330 core

// Interpolated values from the vertex shaders
in vec3 vPos_VS;
 
// Ouput data
out vec4 color;

uniform vec2 textureSize;
uniform sampler2D startingPositionTexture;

void main()
{  
  vec2 uv;
  uv.x = gl_FragCoord.x / textureSize.x;
  uv.y = gl_FragCoord.y / textureSize.y;

  vec4 start = texture ( startingPositionTexture, uv );
  
  vec3 dir_obj = vPos_VS - start.xyz;
  float l =  length(dir_obj);
  vec3 dir = normalize(dir_obj);  
  
  color = vec4(dir, l * start.a ); // normalized dir, length * (active)  
}