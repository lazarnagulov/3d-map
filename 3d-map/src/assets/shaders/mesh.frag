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

    vec3 baseColor = uMaterial.kD;
    if (uUseTexture == 1)
        baseColor = texture(uTexture, vTexCoord).rgb;

    vec3 viewDir = normalize(uViewPos - vWorldPos);

    vec3 result = vec3(0.0);

    for (int i = 0; i < uLightCount; i++)
    {
        vec3 lightDir = normalize(uLights[i].pos - vWorldPos);

        vec3 ambient = uLights[i].kA * baseColor;
        float diff = max(dot(normal, lightDir), 0.0);
        vec3 diffuse = uLights[i].kD * diff * baseColor;
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(
            max(dot(viewDir, reflectDir), 0.0),
            uMaterial.shine
        );

        vec3 specular =
            uLights[i].kS * spec * uMaterial.kS;

        result += ambient + diffuse + specular;
    }

    FragColor = vec4(result, 1.0);
}
