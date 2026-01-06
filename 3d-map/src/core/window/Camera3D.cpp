#include "Camera3D.h"

#include <stdexcept>

Camera3D::Camera3D(const glm::vec3& position /*= { 0 , 0, 0 }*/, float zoom /*= 1.0f*/) {}

void Camera3D::Move(const glm::vec3& delta) {
    m_Position += delta;
}

void Camera3D::Zoom(float dfov) {
    m_Fov = glm::clamp(m_Fov - dfov, 30.0f, 90.0f);
}

glm::mat4 Camera3D::GetViewMatrix() const {
    throw std::logic_error("Function not yet implemented");

}
glm::mat4 Camera3D::GetProjectionMatrix(int screenWidth, int screenHeight) const {
    throw std::logic_error("Function not yet implemented");
}
glm::mat4 Camera3D::GetViewProjection(int screenWidth, int screenHeight) const {
    throw std::logic_error("Function not yet implemented");
}

Ray Camera3D::ScreenToWorld(float x, float y, int screenWidth, int screenHeight) const {
    throw std::logic_error("Function not yet implemented");
}
glm::vec3 Camera3D::WorldToScreen(const glm::vec3& p, int screenWidth, int screenHeight) const {
    throw std::logic_error("Function not yet implemented");
}