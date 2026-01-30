#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include "Shader.h"
#include "buffers/VertexArray.h"
#include "buffers/VertexBuffer.h"
#include "buffers/IndexBuffer.h"
#include "buffers/VertexBufferLayout.h"
#include "Texture.h"
#include "Mesh.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "../window/Camera3D.h"

class Renderer
{
public:
    enum class DepthMode {
        Enabled,
        Disabled
    };

    struct Light {
        glm::vec3 position;
        glm::vec3 kA, kD, kS;
    };
    
    Renderer(const std::shared_ptr<Shader>& shader);

    void SetDepthMode(DepthMode mode);

    void BeginScene(const glm::mat4& viewProjection);
    void BeginScene(const glm::mat4& viewProjection, const glm::vec3 cameraPos);
    void EndScene();

    void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
    void DrawQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color);
    void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Texture& texture);
    void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Texture& texture, const glm::vec4& tint);

    void DrawLine(const glm::vec2& p0, const glm::vec2& p1, float thickness, const glm::vec4& color);

    void DrawCircle(const glm::vec2& center, float radius, const glm::vec4& color, bool filled = true);
    void DrawCircleOutline(const glm::vec2& center, float radius, float thickness, const glm::vec4& color);

    void DrawText(const std::string& txt, glm::vec2 pos, float scale, const glm::vec4& color = glm::vec4(1.0f));
    void LoadFont(const std::string& fontPath, unsigned int fontSize = 48);
    
    std::unique_ptr<Mesh> CreateCubeMesh(float size);
    std::unique_ptr<Mesh> CreateCylinderMesh(float radius, float height, int segments = 16);
    std::unique_ptr<Mesh> CreateSphereMesh(float radius, int segments = 16);

    void DrawMesh(const Mesh& mesh, const glm::mat4& transform);
    inline void SetTextShader(std::shared_ptr<Shader>& shader) { m_TextShader = shader; }
    inline void SetMeshShader(std::shared_ptr<Shader>& shader) { m_MeshShader = shader; }
private:
    void InitQuad();
    void InitLine();
    void InitCircle();

    struct Character {
        unsigned int TextureId;
        glm::ivec2 Size;
        glm::ivec2 Bearing;
        unsigned int Advance;
    };

private:
    Light m_Light;
    
    std::shared_ptr<Shader> m_QuadShader;
    std::shared_ptr<Shader> m_ColorShader;
    std::shared_ptr<Shader> m_CircleShader;
    std::shared_ptr<Shader> m_TextShader;
    std::shared_ptr<Shader> m_MeshShader;

    std::unique_ptr<VertexArray> m_QuadVA;
    std::unique_ptr<VertexBuffer> m_QuadVB;
    std::unique_ptr<IndexBuffer> m_QuadIB;
    bool m_QuadInitialized = false;

    std::unique_ptr<VertexArray> m_LineVA;
    std::unique_ptr<VertexBuffer> m_LineVB;
    std::unique_ptr<IndexBuffer> m_LineIB;
    bool m_LineInitialized = false;

    std::unique_ptr<VertexArray> m_CircleVA;
    std::unique_ptr<VertexBuffer> m_CircleVB;
    std::unique_ptr<IndexBuffer> m_CircleIB;
    bool m_CircleInitialized = false;
    unsigned int m_CircleSegments = 64;

    std::unordered_map<char, Character> m_Characters;
    std::unique_ptr<VertexArray> m_TextVA;
    std::unique_ptr<VertexBuffer> m_TextVB;
    bool m_FontLoaded = false;
    
    glm::mat4 m_ViewProjection = glm::mat4(1.0f);
};