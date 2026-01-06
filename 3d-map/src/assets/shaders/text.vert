#version 330 core

layout (location = 0) in vec4 inVertex;

out vec2 chTexCoords;

uniform mat4 uViewProjection;

void main()
{
    gl_Position = uViewProjection * vec4(inVertex.xy, 0.0, 1.0);
    chTexCoords = inVertex.zw;
}