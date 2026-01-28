#include "Camera3D.h"
#include <stdexcept>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/trigonometric.hpp>

Camera3D::Camera3D(const glm::vec3& position /*= { 0 , 0, 0 }*/, float fov, float yaw /*= -90.0f*/, float pitch /*= 0.0f*/)
    : m_Position(position), m_Zoom(1.0f), m_Yaw(yaw), m_Pitch(pitch), m_Fov(fov) {}

void Camera3D::Move(const glm::vec3& delta) {
    m_Position += delta;
}

void Camera3D::Zoom(float dfov) {
    m_Fov = glm::clamp(m_Fov - dfov, 30.0f, 90.0f);
}

glm::mat4 Camera3D::GetViewMatrix() const {
    glm::vec3 front{ 0 };
    front.x = glm::cos(glm::radians(m_Yaw)) * glm::cos(glm::radians(m_Pitch));
    front.y = glm::sin(glm::radians(m_Pitch));
    front.z = glm::sin(glm::radians(m_Yaw)) * glm::cos(glm::radians(m_Pitch));
    glm::vec3 direction = glm::normalize(front);
    glm::vec3 up = { 0.0f, 1.0f, 0.0f };
    return glm::lookAt(m_Position, m_Position + direction, up);
}

glm::mat4 Camera3D::GetProjectionMatrix(int screenWidth, int screenHeight) const {
    float aspect = float(screenWidth) / float(screenHeight);
    return glm::perspective(
        glm::radians(m_Fov),
        aspect,
        m_Near,
        m_Far
    );
}

glm::mat4 Camera3D::GetViewProjection(int screenWidth, int screenHeight) const {
    return GetProjectionMatrix(screenWidth, screenHeight) * GetViewMatrix();
}


Ray Camera3D::ScreenToWorld(float x, float y, int w, int h) const {
    float nx = 2.0f * x / w - 1.0f;
    float ny = 2.0f * y / h - 1.0f;

    glm::mat4 invVP = glm::inverse(GetViewProjection(w, h));

    glm::vec4 nearClip(nx, ny, -1.0f, 1.0f);
    glm::vec4 farClip(nx, ny, 1.0f, 1.0f);

    glm::vec4 nearWorld = invVP * nearClip;
    glm::vec4 farWorld = invVP * farClip;

    nearWorld /= nearWorld.w;
    farWorld /= farWorld.w;

    glm::vec3 origin = glm::vec3(nearWorld);
    glm::vec3 dir = glm::normalize(glm::vec3(farWorld - nearWorld));

    return { origin, dir };
}

glm::vec2 Camera3D::WorldToScreen(const glm::vec3& p, int screenWidth, int screenHeight) const {
    glm::vec4 clip = GetViewProjection(screenWidth, screenHeight) * glm::vec4(p, 1.0f);
    clip /= clip.w;
    float sx = (clip.x * 0.5f + 0.5f) * screenWidth;
    float sy = (clip.y * 0.5f + 0.5f) * screenHeight;
    return { sx, sy };
}