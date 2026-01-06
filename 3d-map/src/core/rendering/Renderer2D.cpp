#include "Renderer2D.h"
#include <GL/glew.h>
#include <iostream>

#include "ft2build.h"
#include <freetype/freetype.h>
#define FT_FREETYPE_H

Renderer2D::Renderer2D(const std::shared_ptr<Shader>& shader)
    : m_QuadShader(shader) { }

void Renderer2D::BeginScene(const glm::mat4& viewProjection) {
    m_ViewProjection = viewProjection;

    if (m_QuadShader) {
        m_QuadShader->Bind();
        m_QuadShader->SetUniformMat4("uViewProjection", m_ViewProjection);
    }
}

void Renderer2D::EndScene() { }

void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color) {
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

void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color) {
    DrawQuad(position, size, 0.0f, color);
}

void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Texture& texture, const glm::vec4& tint) {
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

void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Texture& texture) {
    DrawQuad(position, size, texture, glm::vec4(1.0f));
}

void Renderer2D::DrawLine(const glm::vec2& p0, const glm::vec2& p1, float thickness, const glm::vec4& color) {
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

void Renderer2D::DrawCircle(const glm::vec2& center, float radius, const glm::vec4& color, bool filled) {
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

void Renderer2D::DrawCircleOutline(const glm::vec2& center, float radius, float thickness, const glm::vec4& color) {
    const int segments = 64;

    for (int i = 0; i < segments; ++i) {
        float angle1 = (float)i / (float)segments * 2.0f * 3.14159265359f;
        float angle2 = (float)(i + 1) / (float)segments * 2.0f * 3.14159265359f;

        glm::vec2 p1 = center + glm::vec2(std::cos(angle1), std::sin(angle1)) * radius;
        glm::vec2 p2 = center + glm::vec2(std::cos(angle2), std::sin(angle2)) * radius;

        DrawLine(p1, p2, thickness, color);
    }
}

void Renderer2D::InitQuad() {
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


void Renderer2D::InitLine() {
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

void Renderer2D::InitCircle() {
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

void Renderer2D::DrawText(const std::string& txt, glm::vec2 pos, float scale, const glm::vec4& color) {
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

void Renderer2D::LoadFont(const std::string& fontPath, unsigned int fontSize) {
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