#include "Mesh.h"

Mesh::Mesh(
    const std::vector<Vertex>& vertices,
    const std::vector<uint32_t>& indices
) {
    SetupMesh(vertices, indices);
}

void Mesh::SetTexture(std::shared_ptr<Texture> texture) {
    m_Texture = texture;
}

void Mesh::SetColor(const glm::vec4& color) {
    m_Color = color;
}

void Mesh::SetupMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) {
    m_IndexCount = static_cast<uint32_t>(indices.size());

    m_VAO = std::make_unique<VertexArray>();
    m_VBO = std::make_unique<VertexBuffer>(
        vertices.data(),
        vertices.size() * sizeof(Vertex)
    );

    VertexBufferLayout layout;
    layout.PushFloat(3); // position
    layout.PushFloat(3); // normal
    layout.PushFloat(2); // texCoord

    m_VAO->AddBuffer(*m_VBO, layout);

    if (!indices.empty()) {
        m_IBO = std::make_unique<IndexBuffer>(
            indices.data(),
            indices.size()
        );
    }
}

void Mesh::Draw() const {
    m_VAO->Bind();

    if (m_IBO) {
        m_IBO->Bind();
        glDrawElements(GL_TRIANGLES, m_IndexCount, GL_UNSIGNED_INT, nullptr);
    }
    else {
        glDrawArrays(GL_TRIANGLES, 0, m_IndexCount);
    }
}