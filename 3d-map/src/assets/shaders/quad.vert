#version 330 core

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inTexCoord;

out vec2 chTexCoord;

uniform mat4 uViewProjection;
uniform mat4 uModel;

void main()
{
    gl_Position = uViewProjection * uModel * vec4(inPosition, 0.0, 1.0);
    chTexCoord = inTexCoord;
}