#pragma once

#include <vector>
#include <memory>
#include <glm/glm.hpp>

#include "./buffers/VertexArray.h"
#include "./buffers/VertexBuffer.h"
#include "./buffers/IndexBuffer.h"
#include "./Texture.h"

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
};

struct Material {
    glm::vec3 kA;
    glm::vec3 kD;
    glm::vec3 kS;
    float shine;

    Material() = default;

    Material(glm::vec3 kA, glm::vec3 kD, glm::vec3 kS, float shine)
        : kA(kA), kD(kD), kS(kS), shine(shine) {}
};

class Mesh {
public:
    Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);

    inline void SetTexture(std::shared_ptr<Texture> texture) { m_Texture = texture;  }
    inline void SetMaterial(const Material& material) { m_Material = material; }

    inline bool HasTexture() const { return m_Texture != nullptr; }
    inline const Texture& GetTexture() const { return *m_Texture; }
    inline const Material& GetMaterial() const { return m_Material; }

    void Draw() const;

private:
    void SetupMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
private:
    std::unique_ptr<VertexArray>  m_VAO;
    std::unique_ptr<VertexBuffer> m_VBO;
    std::unique_ptr<IndexBuffer>  m_IBO;

    uint32_t m_IndexCount = 0;
    Material m_Material;

    std::shared_ptr<Texture> m_Texture;
};