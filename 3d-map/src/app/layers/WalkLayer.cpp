#include "WalkLayer.h"
#include "GLFW/glfw3.h"

WalkLayer::WalkLayer(Input& input, Camera2D& camera) 
    : m_Input(input), m_Camera(camera) {}

void WalkLayer::OnUpdate(float dt) {
    uint8_t directions = 0;
    if (m_Input.IsKeyHeld(GLFW_KEY_S))
        directions |= WalkState::MoveDirection::UP;
    if (m_Input.IsKeyHeld(GLFW_KEY_W))
        directions |= WalkState::MoveDirection::DOWN;
    if (m_Input.IsKeyHeld(GLFW_KEY_A))
        directions |= WalkState::MoveDirection::LEFT;
    if (m_Input.IsKeyHeld(GLFW_KEY_D))
        directions |= WalkState::MoveDirection::RIGHT;

    glm::vec2 oldPos = m_State.GetPosition();

    m_State.Update(dt, directions);

    glm::vec2 newPos = m_State.GetPosition();
    glm::vec2 movement = newPos - oldPos;

    if (glm::length(movement) > 0.001f) {
        m_TargetRotation = glm::degrees(atan2(movement.y, -movement.x));

        float rotationDiff = m_TargetRotation - m_PlayerRotation;

        while (rotationDiff > 180.0f) rotationDiff -= 360.0f;
        while (rotationDiff < -180.0f) rotationDiff += 360.0f;

        m_PlayerRotation += rotationDiff * glm::min(10.0f * dt, 1.0f);

        if (m_PlayerRotation < 0.0f) m_PlayerRotation += 360.0f;
        if (m_PlayerRotation >= 360.0f) m_PlayerRotation -= 360.0f;
    }

    m_Camera.SetPosition(m_State.GetPosition());
}
void WalkLayer::OnRender(Renderer& renderer) {
    glm::vec2 pos2D = m_State.GetPosition();
    glm::vec3 pos3D(pos2D.x, 0.0, pos2D.y);

    if (!m_PlayerModel) {
        m_PlayerModel = std::make_unique<Model>("./src/assets/models/player.obj");
    
        Material playerMat;
        playerMat.kA = { 0.1f, 0.1f, 0.1f };
        playerMat.kD = { 0.2f, 0.6f, 1.0f };
        playerMat.kS = { 0.5f, 0.5f, 0.5f };
        playerMat.shine = 64.0f;

        m_PlayerModel->SetMaterial(playerMat);
    }

    glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos3D);
    transform = glm::rotate(transform, glm::radians(m_PlayerRotation - 90.0f), glm::vec3(0, 1, 0));
    transform = glm::scale(transform, glm::vec3(7.5f)); 
    
    renderer.DrawModel(*m_PlayerModel, transform);
}

void WalkLayer::OnKey(int key, int action) {
    if (key == GLFW_KEY_0 && action == GLFW_PRESS) {
        m_State.Reset();
    }
}