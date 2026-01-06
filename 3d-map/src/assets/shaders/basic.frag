#version 330 core

in vec4 channelCol;
in vec2 chTexCoord;
out vec4 FragColor;

uniform sampler2D uTexture;

void main()
{
	FragColor = texture(uTexture, chTexCoord) * channelCol;
}