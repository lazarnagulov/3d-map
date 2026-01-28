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

class Mesh {
public:
    Mesh(
        const std::vector<Vertex>& vertices,
        const std::vector<uint32_t>& indices
    );

    void SetTexture(std::shared_ptr<Texture> texture);
    void SetColor(const glm::vec4& color);

    bool HasTexture() const { return m_Texture != nullptr; }
    const Texture& GetTexture() const { return *m_Texture; }
    const glm::vec4& GetColor() const { return m_Color; }

    void Draw() const;

private:
    void SetupMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);
private:
    std::unique_ptr<VertexArray>  m_VAO;
    std::unique_ptr<VertexBuffer> m_VBO;
    std::unique_ptr<IndexBuffer>  m_IBO;

    uint32_t m_IndexCount = 0;

    std::shared_ptr<Texture> m_Texture;
    glm::vec4 m_Color = { 1.0f, 1.0f, 1.0f, 1.0f };
};