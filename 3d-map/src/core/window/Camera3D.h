#pragma once

#include <glm/glm.hpp>

struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;
};

class Camera3D {
public:
    Camera3D(const glm::vec3& position = { 0 , 0, 0 }, float zoom = 1.0f);

    void Move(const glm::vec3& delta);
    void Zoom(float dfov);

    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix(int screenWidth, int screenHeight) const;
    glm::mat4 GetViewProjection(int screenWidth, int screenHeight) const;

    Ray ScreenToWorld(float x, float y, int screenWidth, int screenHeight) const;
    glm::vec2 WorldToScreen(const glm::vec3& p, int screenWidth, int screenHeight) const;

    inline float GetZoom() const { return m_Zoom; }
    inline glm::vec3 GetPosition() { return m_Position; }
    inline float GetYaw() { return m_Yaw; }
    inline float GetPitch() { return m_Pitch; }

    inline void SetPosition(const glm::vec3& pos) { m_Position = pos; }
    inline void SetZoom(float zoom) { m_Zoom = std::max(0.001f, zoom); }
    inline void SetYaw(float value) { m_Yaw = value; }
    inline void SetPitch(float value) { m_Pitch = value; }
private:
    glm::vec3 m_Position = { 0, 0, 0 };
    float m_Zoom = 1.0f;
    float m_Fov = 60.0f;
    float m_Near = 0.1f;
    float m_Far = 5000.0f;
    
    float m_Yaw = -90.0f;
    float m_Pitch = 0.0f;
};
