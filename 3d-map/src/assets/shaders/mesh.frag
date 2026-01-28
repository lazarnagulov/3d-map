#version 330 core

uniform sampler2D uTexture;
uniform vec4 uColor;
uniform int uUseTexture;

in vec2 vTexCoord;
out vec4 FragColor;

void main()
{
    if (uUseTexture == 1)
        FragColor = texture(uTexture, vTexCoord);
    else
        FragColor = uColor;
}