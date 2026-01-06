#version 330 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec4 inCol;
layout(location = 2) in vec2 inTexCoord;

uniform mat4 uM; //Matrica transformacije
uniform mat4 uV; //Matrica kamere
uniform mat4 uP; //Matrica projekcija

out vec2 chTexCoord;
out vec4 channelCol;

void main()
{
	gl_Position = uP * uV * uM * vec4(inPos, 1.0); //Zbog nekomutativnosti mnozenja matrica, moramo mnoziti MVP matrice i tjemena "unazad"
	chTexCoord = inTexCoord;
	channelCol = inCol;
}