#version 330 core

in vec2 chTexCoords;
out vec4 color;

uniform sampler2D uText;
uniform vec4 uTextColor;

void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(uText, chTexCoords).r);
    color = uTextColor * sampled;
}