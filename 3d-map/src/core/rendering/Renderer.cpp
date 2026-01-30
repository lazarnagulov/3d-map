#include "Renderer.h"
#include <GL/glew.h>
#include <iostream>

#include "ft2build.h"
#include <freetype/freetype.h>
#define FT_FREETYPE_H

Renderer::Renderer(const std::shared_ptr<Shader>& shader)
    : m_QuadShader(shader) { }

void Renderer::BeginScene(const glm::mat4& viewProjection) {
    m_ViewProjection = viewProjection;

    if (m_QuadShader) {
        m_QuadShader->Bind();
        m_QuadShader->SetUniformMat4("uViewProjection", m_ViewProjection);
    }
}

void Renderer::BeginScene(const glm::mat4& viewProjection, const glm::vec3 cameraPos) {
    m_ViewProjection = viewProjection;

    if (m_QuadShader) {
        m_QuadShader->Bind();
        m_QuadShader->SetUniformMat4("uViewProjection", m_ViewProjection);
    }

    if (m_MeshShader) {
        m_MeshShader->Bind();

        m_MeshShader->SetUniformVec3("uViewPos", cameraPos);
        m_MeshShader->SetUniform1i("uLightCount", 1);
        
        m_MeshShader->SetUniformVec3("uLights[0].pos", { 0.0f, 10000.0f, 0.0f });
        m_MeshShader->SetUniformVec3("uLights[0].kA", { 0.3f, 0.3f, 0.3f });
        m_MeshShader->SetUniformVec3("uLights[0].kD", { 0.6f, 0.6f, 0.6f });
        m_MeshShader->SetUniformVec3("uLights[0].kS", { 0.2f, 0.2f, 0.2f });
        
        m_MeshShader->SetUniformVec3("uMaterial.kA", glm::vec3(0.2f));
        m_MeshShader->SetUniformVec3("uMaterial.kD", glm::vec3(1.0f));
        m_MeshShader->SetUniformVec3("uMaterial.kS", glm::vec3(0.0f));
        m_MeshShader->SetUniform1f("uMaterial.shine", 1.0f);
    }
}

void Renderer::EndScene() { }

void Renderer::UploadLights(const std::vector<PointLight>& lights) {
    m_MeshShader->Bind();

    int count = 0;

    for (const auto& l : lights)
    {
        if (count >= 16) break;

        glm::vec3 color = l.color * l.intensity;

        m_MeshShader->SetUniformVec3(
            "uLights[" + std::to_string(count) + "].pos",
            l.position
        );

        m_MeshShader->SetUniformVec3(
            "uLights[" + std::to_string(count) + "].kA",
            color * 0.15f
        );

        m_MeshShader->SetUniformVec3(
            "uLights[" + std::to_string(count) + "].kD",
            color
        );

        m_MeshShader->SetUniformVec3(
            "uLights[" + std::to_string(count) + "].kS",
            color
        );

        count++;
    }

    m_MeshShader->SetUniform1i("uLightCount", count);
}


void Renderer::DrawQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color) {
    if (!m_QuadInitialized)
        InitQuad();

    m_QuadShader->Bind();

    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(position, 0.0f));

    if (rotation != 0.0f)
        model = glm::rotate(model, rotation, glm::vec3(0.0f, 0.0f, 1.0f));

    model = glm::scale(model, glm::vec3(size, 1.0f));

    m_QuadShader->SetUniformMat4("uModel", model);
    m_QuadShader->SetUniform4f("uColor", color);
    m_QuadShader->SetUniform1i("uTex", 0);

    m_QuadVA->Bind();
    m_QuadIB->Bind();

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

void Renderer::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color) {
    DrawQuad(position, size, 0.0f, color);
}

void Renderer::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Texture& texture, const glm::vec4& tint) {
    if (!m_QuadInitialized)
        InitQuad();

    m_QuadShader->Bind();

    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(position, 0.0f))
        * glm::scale(glm::mat4(1.0f), glm::vec3(size, 1.0f));

    m_QuadShader->SetUniformMat4("uModel", model);
    m_QuadShader->SetUniform4f("uColor", tint);
    m_QuadShader->SetUniform1i("uTex", 1);

    texture.Bind(0);

    m_QuadVA->Bind();
    m_QuadIB->Bind();

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

void Renderer::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Texture& texture) {
    DrawQuad(position, size, texture, glm::vec4(1.0f));
}

void Renderer::DrawLine(const glm::vec2& p0, const glm::vec2& p1, float thickness, const glm::vec4& color) {
    if (!m_LineInitialized)
        InitLine();

    glm::vec2 direction = p1 - p0;
    float length = glm::length(direction);

    if (length < 0.0001f)
        return;

    glm::vec2 mid = p0 + direction * 0.5f;
    float angle = std::atan2(direction.y, direction.x);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(mid, 0.0f));
    model = glm::rotate(model, angle, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(length, thickness, 1.0f));

    m_QuadShader->Bind();
    m_QuadShader->SetUniformMat4("uModel", model);
    m_QuadShader->SetUniform4f("uColor", color);
    m_QuadShader->SetUniform1i("uTex", 0);

    m_LineVA->Bind();
    m_LineIB->Bind();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

void Renderer::DrawCircle(const glm::vec2& center, float radius, const glm::vec4& color, bool filled) {
    if (filled) {
        if (!m_CircleInitialized)
            InitCircle();

        m_QuadShader->Bind();

        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(center, 0.0f))
            * glm::scale(glm::mat4(1.0f), glm::vec3(radius * 2.0f, radius * 2.0f, 1.0f));

        m_QuadShader->SetUniformMat4("uModel", model);
        m_QuadShader->SetUniform4f("uColor", color);
        m_QuadShader->SetUniform1i("uTex", 0);

        m_CircleVA->Bind();
        m_CircleIB->Bind();

        glDrawElements(GL_TRIANGLES, m_CircleSegments * 3, GL_UNSIGNED_INT, nullptr);
    }
    else {
        DrawCircleOutline(center, radius, 2.0f, color);
    }
}

void Renderer::DrawCircleOutline(const glm::vec2& center, float radius, float thickness, const glm::vec4& color) {
    const int segments = 64;

    for (int i = 0; i < segments; ++i) {
        float angle1 = (float)i / (float)segments * 2.0f * 3.14159265359f;
        float angle2 = (float)(i + 1) / (float)segments * 2.0f * 3.14159265359f;

        glm::vec2 p1 = center + glm::vec2(std::cos(angle1), std::sin(angle1)) * radius;
        glm::vec2 p2 = center + glm::vec2(std::cos(angle2), std::sin(angle2)) * radius;

        DrawLine(p1, p2, thickness, color);
    }
}

void Renderer::InitQuad() {
    float vertices[] = {
        -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.0f, 1.0f
    };

    unsigned int indices[] = { 0,1,2, 2,3,0 };

    m_QuadVA = std::make_unique<VertexArray>();
    m_QuadVB = std::make_unique<VertexBuffer>(vertices, sizeof(vertices));
    m_QuadIB = std::make_unique<IndexBuffer>(indices, 6);

    VertexBufferLayout layout;
    layout.PushFloat(2);
    layout.PushFloat(2);

    m_QuadVA->AddBuffer(*m_QuadVB, layout);

    m_QuadInitialized = true;
}


void Renderer::InitLine() {
    float vertices[] = {
        -0.5f, -0.5f,
         0.5f, -0.5f,
         0.5f,  0.5f,
        -0.5f,  0.5f
    };

    unsigned int indices[] = { 0,1,2, 2,3,0 };

    m_LineVA = std::make_unique<VertexArray>();
    m_LineVB = std::make_unique<VertexBuffer>(vertices, sizeof(vertices));
    m_LineIB = std::make_unique<IndexBuffer>(indices, 6);

    VertexBufferLayout layout;
    layout.PushFloat(2); 
    m_LineVA->AddBuffer(*m_LineVB, layout);

    m_LineInitialized = true;
}

void Renderer::InitCircle() {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    vertices.push_back(0.0f);
    vertices.push_back(0.0f);

    for (unsigned int i = 0; i <= m_CircleSegments; ++i) {
        float angle = (float)i / (float)m_CircleSegments * 2.0f * 3.14159265359f;
        float x = std::cos(angle) * 0.5f;
        float y = std::sin(angle) * 0.5f;
        vertices.push_back(x);
        vertices.push_back(y);
    }

    for (unsigned int i = 1; i <= m_CircleSegments; ++i) {
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back(i + 1);
    }

    m_CircleVA = std::make_unique<VertexArray>();
    m_CircleVB = std::make_unique<VertexBuffer>(vertices.data(), vertices.size() * sizeof(float));
    m_CircleIB = std::make_unique<IndexBuffer>(indices.data(), indices.size());

    VertexBufferLayout layout;
    layout.PushFloat(2);
    m_CircleVA->AddBuffer(*m_CircleVB, layout);

    m_CircleInitialized = true;
}

void Renderer::DrawText(const std::string& txt, glm::vec2 pos, float scale, const glm::vec4& color) {
    if (!m_FontLoaded) {
        std::cerr << "ERROR: Font not loaded. Call LoadFont() first." << std::endl;
        return;
    }

    if (!m_TextShader) {
        std::cerr << "ERROR: Text shader not set." << std::endl;
        return;
    }

    m_TextShader->Bind();
    m_TextShader->SetUniform4f("uTextColor", color);
    m_TextShader->SetUniformMat4("uViewProjection", m_ViewProjection);


    glActiveTexture(GL_TEXTURE0);
    m_TextVA->Bind();

    for (char c : txt) {
        Character ch = m_Characters[c];

        float xpos = pos.x + ch.Bearing.x * scale;
        float ypos = pos.y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;

        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };


        glBindTexture(GL_TEXTURE_2D, ch.TextureId);

        m_TextVB->Bind();
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        pos.x += (ch.Advance >> 6) * scale;
    }
}

void Renderer::LoadFont(const std::string& fontPath, unsigned int fontSize) {
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        std::cerr << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return;
    }

    FT_Face face;
    if (FT_New_Face(ft, fontPath.c_str(), 0, &face)) {
        std::cerr << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return;
    }

    FT_Set_Pixel_Sizes(face, 0, fontSize);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (unsigned char c = 0; c < 128; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            std::cerr << "ERROR::FREETYPE: Failed to load Glyph " << c << std::endl;
            continue;
        }

        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<unsigned int>(face->glyph->advance.x)
        };
        m_Characters.insert(std::pair<char, Character>(c, character));
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    m_TextVA = std::make_unique<VertexArray>();
    m_TextVB = std::make_unique<VertexBuffer>(nullptr, sizeof(float) * 6 * 4, true); 

    VertexBufferLayout layout;
    layout.PushFloat(4); 
    m_TextVA->AddBuffer(*m_TextVB, layout);

    m_FontLoaded = true;
}

void Renderer::SetDepthMode(DepthMode mode) {
    if (mode == DepthMode::Enabled)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
}

void Renderer::DrawMesh(const Mesh& mesh, const glm::mat4& transform) {
    m_MeshShader->Bind();

    m_MeshShader->SetUniformMat4("uModel", transform);
    m_MeshShader->SetUniformMat4("uViewProjection", m_ViewProjection);

    const Material& mat = mesh.GetMaterial();

    m_MeshShader->SetUniformVec3("uMaterial.kA", mat.kA);
    m_MeshShader->SetUniformVec3("uMaterial.kD", mat.kD);
    m_MeshShader->SetUniformVec3("uMaterial.kS", mat.kS);
    m_MeshShader->SetUniform1f("uMaterial.shine", mat.shine);

    if (mesh.HasTexture()) {
        mesh.GetTexture().Bind(0);
        m_MeshShader->SetUniform1i("uUseTexture", 1);
        m_MeshShader->SetUniform1i("uTexture", 0);
    }
    else {
        m_MeshShader->SetUniform1i("uUseTexture", 0);
    }

    mesh.Draw();
}

void Renderer::DrawModel(const Model& model, const glm::mat4& transform) {
    m_MeshShader->Bind();
    m_MeshShader->SetUniformMat4("uModel", transform);
    m_MeshShader->SetUniformMat4("uViewProjection", m_ViewProjection);
    model.Draw(*m_MeshShader);
}

std::unique_ptr<Mesh> Renderer::CreateCubeMesh(float size) {
    float s = size * 0.5f;
    std::vector<Vertex> vertices = {
        {{-s,-s, s}, {0,0,1}, {0,0}}, {{ s,-s, s}, {0,0,1}, {1,0}},
        {{ s, s, s}, {0,0,1}, {1,1}}, {{-s, s, s}, {0,0,1}, {0,1}},
        {{ s,-s,-s}, {0,0,-1}, {0,0}}, {{-s,-s,-s}, {0,0,-1}, {1,0}},
        {{-s, s,-s}, {0,0,-1}, {1,1}}, {{ s, s,-s}, {0,0,-1}, {0,1}},
        {{-s, s, s}, {0,1,0}, {0,0}}, {{ s, s, s}, {0,1,0}, {1,0}},
        {{ s, s,-s}, {0,1,0}, {1,1}}, {{-s, s,-s}, {0,1,0}, {0,1}},
        {{-s,-s,-s}, {0,-1,0}, {0,0}}, {{ s,-s,-s}, {0,-1,0}, {1,0}},
        {{ s,-s, s}, {0,-1,0}, {1,1}}, {{-s,-s, s}, {0,-1,0}, {0,1}},
        {{ s,-s, s}, {1,0,0}, {0,0}}, {{ s,-s,-s}, {1,0,0}, {1,0}},
        {{ s, s,-s}, {1,0,0}, {1,1}}, {{ s, s, s}, {1,0,0}, {0,1}},
        {{-s,-s,-s}, {-1,0,0}, {0,0}}, {{-s,-s, s}, {-1,0,0}, {1,0}},
        {{-s, s, s}, {-1,0,0}, {1,1}}, {{-s, s,-s}, {-1,0,0}, {0,1}}
    };

    std::vector<uint32_t> indices;
    for (int i = 0; i < 6; i++) {
        unsigned int base = i * 4;
        indices.insert(indices.end(), {
            base + 0, base + 1, base + 2,
            base + 2, base + 3, base + 0
            });
    }

    return std::make_unique<Mesh>(vertices, indices);
}

std::unique_ptr<Mesh> Renderer::CreateSphereMesh(float radius, int segments) {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    for (int lat = 0; lat <= segments; lat++) {
        float theta = lat * 3.14159265359f / segments;
        float sinTheta = sin(theta);
        float cosTheta = cos(theta);

        for (int lon = 0; lon <= segments; lon++) {
            float phi = lon * 2.0f * 3.14159265359f / segments;
            float sinPhi = sin(phi);
            float cosPhi = cos(phi);

            glm::vec3 pos(
                radius * sinTheta * cosPhi,
                radius * cosTheta,
                radius * sinTheta * sinPhi
            );
            glm::vec3 normal = glm::normalize(pos);
            glm::vec2 uv((float)lon / segments, (float)lat / segments);

            vertices.push_back({ pos, normal, uv });
        }
    }

    for (int lat = 0; lat < segments; lat++) {
        for (int lon = 0; lon < segments; lon++) {
            unsigned int first = lat * (segments + 1) + lon;
            unsigned int second = first + segments + 1;

            indices.insert(indices.end(), {
                first, second, first + 1,
                second, second + 1, first + 1
            });
        }
    }

    return std::make_unique<Mesh>(vertices, indices);
}

std::unique_ptr<Mesh> Renderer::CreateCylinderMesh(float radius, float height, int segments) {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    vertices.push_back({ {0, 0, 0}, {0, -1, 0}, {0.5f, 0.5f} });
    vertices.push_back({ {0, height, 0}, {0, 1, 0}, {0.5f, 0.5f} });

    for (int i = 0; i <= segments; i++) {
        float angle = (float)i / segments * 2.0f * 3.14159265359f;
        float x = cos(angle) * radius;
        float z = sin(angle) * radius;
        glm::vec3 normal = glm::normalize(glm::vec3(x, 0, z));

        vertices.push_back({ {x, 0, z}, normal, {(float)i / segments, 0} });
        vertices.push_back({ {x, height, z}, normal, {(float)i / segments, 1} });
    }

    for (int i = 0; i < segments; i++) {
        indices.insert(indices.end(), { 0, static_cast<unsigned int>(2 + i * 2 + 2), static_cast<unsigned int>(2 + i * 2) });
    }

    for (int i = 0; i < segments; i++) {
        indices.insert(indices.end(), { 1, static_cast<unsigned int>(3 + i * 2), static_cast<unsigned int>(3 + i) * 2 + 2 });
    }

    for (int i = 0; i < segments; i++) {
        unsigned int b1 = 2 + i * 2;
        unsigned int t1 = 3 + i * 2;
        unsigned int b2 = 2 + i * 2 + 2;
        unsigned int t2 = 3 + i * 2 + 2;

        indices.insert(indices.end(), { b1, b2, t1, t1, b2, t2 });
    }

    return std::make_unique<Mesh>(vertices, indices);
}