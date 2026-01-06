#pragma once

#include <glm/glm.hpp>

class Camera {
public:
    Camera(const glm::vec2& position = { 0 , 0 }, float zoom = 1.0f);

    void Move(const glm::vec2& delta);
    void Zoom(float dz);

    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix(int screenWidth, int screenHeight) const;
    glm::mat4 GetViewProjection(int screenWidth, int screenHeight) const;

    glm::vec2 ScreenToWorld(float x, float y, int screenWidth, int screenHeight) const;
    glm::vec2 WorldToScreen(const glm::vec2& p, int screenWidth, int screenHeight) const;

    inline float GetZoom() const { return m_Zoom;  }
    inline glm::vec2 GetPosition() { return m_Position; }

    inline void SetPosition(const glm::vec2& pos) { m_Position = pos; }
    inline void SetZoom(float zoom) { m_Zoom = std::max(0.001f, zoom); }
private:
    glm::vec2 m_Position = { 0, 0 };
    float m_Zoom = 1.0f;
};