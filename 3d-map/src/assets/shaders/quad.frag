#version 330 core

in vec2 chTexCoord;
out vec4 FragColor;

uniform vec4 uColor;
uniform int uTex;
uniform sampler2D uTexture;

void main()
{
    if (uTex == 1) {
        FragColor = texture(uTexture, chTexCoord) * uColor;
    } else {
        FragColor = uColor;
    }
}