#version 330 core

#define MAX_LIGHTS 16

struct Light {
    vec3 pos;
    vec3 kA;
    vec3 kD;
    vec3 kS;
};

struct Material {
    vec3 kA;
    vec3 kD;
    vec3 kS;
    float shine;
};

in vec3 vWorldPos;
in vec3 vNormal;
in vec2 vTexCoord;

out vec4 FragColor;

uniform vec3 uViewPos;

uniform sampler2D uTexture;
uniform int uUseTexture;

uniform Material uMaterial;

uniform int uLightCount;
uniform Light uLights[MAX_LIGHTS];

void main()
{
    vec3 normal = normalize(vNormal);
    vec3 baseColor = (uUseTexture == 1) ? texture(uTexture, vTexCoord).rgb : uMaterial.kD;
    vec3 viewDir = normalize(uViewPos - vWorldPos);

    vec3 result = baseColor * 0.2; 

    for (int i = 0; i < uLightCount; i++)
    {
        vec3 lightDir;
        float attenuation = 1.0;

        if (i == 0) {
            lightDir = normalize(uLights[i].pos); 
            attenuation = 1.0;
        } else {
            lightDir = normalize(uLights[i].pos - vWorldPos);
            float dist = length(uLights[i].pos - vWorldPos);
            attenuation = 1.0 / (1.0 + 0.02 * dist + 0.002 * (dist * dist));
        }

        float diff = max(dot(normal, lightDir), 0.0);
        vec3 diffuse = uLights[i].kD * diff * baseColor * attenuation;

        result += diffuse;
    }

    FragColor = vec4(result, 1.0);
}