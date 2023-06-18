#version 430 core

out vec4 fColor;

uniform vec2 textureSize;
uniform sampler2D imageTexture;
uniform float opacity;

void main()
{
	//Note: textureSize = [0,1]; texture_uv = [-1,1]
	vec2 texture_uv;
	texture_uv.x = gl_FragCoord.x / textureSize.x;
	texture_uv.y = gl_FragCoord.y / textureSize.y;
	fColor = texture( imageTexture, texture_uv ) * opacity;	
}