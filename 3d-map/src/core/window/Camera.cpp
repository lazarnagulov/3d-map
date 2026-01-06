#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(const glm::vec2& position /*= { 0 , 0 }*/, float zoom /*= 1.0f*/)
	: m_Position(position), m_Zoom(zoom) {}

void Camera::Move(const glm::vec2& delta) {
    m_Position += delta;
}

void Camera::Zoom(float dz) {
    m_Zoom = std::max(0.001f, m_Zoom + dz);
}

glm::mat4 Camera::GetViewMatrix() const {
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::scale(view, glm::vec3(m_Zoom, m_Zoom, 1.0f));
    view = glm::translate(view, glm::vec3(-m_Position, 0.0f));
    return view;
}

glm::mat4 Camera::GetProjectionMatrix(int screenWidth, int screenHeight) const {
    float halfW = screenWidth * 0.5f / m_Zoom;
    float halfH = screenHeight * 0.5f / m_Zoom;
    return glm::ortho(-halfW, halfW, -halfH, halfH, -1.0f, 1.0f);
}


glm::mat4 Camera::GetViewProjection(int screenWidth, int screenHeight) const {
    return GetProjectionMatrix(screenWidth, screenHeight) * GetViewMatrix();
}

glm::vec2 Camera::ScreenToWorld(float x, float y, int screenWidth, int screenHeight) const {
    float nx = 2.0f * x / screenWidth - 1.0f;
    float ny = 1.0f - 2.0f * y / screenHeight;

    glm::vec4 clip(nx, ny, 0.0f, 1.0f);
    glm::mat4 invVP = glm::inverse(GetProjectionMatrix(screenWidth, screenHeight) * GetViewMatrix());
    glm::vec4 world = invVP * clip;
    
    return glm::vec2(world.x, world.y);
}

glm::vec2 Camera::WorldToScreen(const glm::vec2& p, int screenWidth, int screenHeight) const {
    glm::vec4 clip = GetViewProjection(screenWidth, screenHeight) * glm::vec4(p, 0.0f, 1.0f);

    float sx = (clip.x * 0.5f + 0.5f) * screenWidth;
    float sy = (1.0f - (clip.y * 0.5f + 0.5f)) * screenHeight;

    return { sx, sy };
}