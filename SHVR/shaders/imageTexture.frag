#version 430 core

in vec2 UV;
out vec4 fColor;

uniform sampler2D imageTexture;
uniform float opacity;

void main()
{
	fColor = texture( imageTexture, UV ) * opacity;
}