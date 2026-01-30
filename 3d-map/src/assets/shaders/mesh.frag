#version 330 core

struct Light {
    vec3 pos;
    vec3 kA; // ambient
    vec3 kD; // diffuse
    vec3 kS; // specular
};

struct Material {
    vec3 kA;
    vec3 kD;
    vec3 kS;
    float shine;
};

in vec3 vFragPos;
in vec3 vNormal;
in vec2 vTexCoord;

out vec4 FragColor;

uniform Light uLight;
uniform Material uMaterial;
uniform vec3 uViewPos;

uniform sampler2D uTexture;
uniform int uUseTexture;

void main()
{
    vec3 normal = normalize(vNormal);

    vec3 lightDir = normalize(uLight.pos - vFragPos);

    vec3 baseColor = uMaterial.kD;
    if (uUseTexture == 1)
        baseColor = texture(uTexture, vTexCoord).rgb;

    vec3 ambient = uLight.kA * baseColor;

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = uLight.kD * diff * baseColor;

    vec3 viewDir = normalize(uViewPos - vFragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), uMaterial.shine);
    vec3 specular = uLight.kS * spec * uMaterial.kS;

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}
