#include "WalkLayer.h"
#include "GLFW/glfw3.h"

WalkLayer::WalkLayer(Input& input, Camera& camera) 
    : m_Input(input), m_Camera(camera), m_MapPinTexture("./src/assets/textures/map-pin.png") {}

void WalkLayer::OnUpdate(float dt) {
    uint8_t directions = 0;
    if (m_Input.IsKeyHeld(GLFW_KEY_W))
        directions |= WalkState::MoveDirection::UP;
    if (m_Input.IsKeyHeld(GLFW_KEY_S))
        directions |= WalkState::MoveDirection::DOWN;
    if (m_Input.IsKeyHeld(GLFW_KEY_A)) 
        directions |= WalkState::MoveDirection::LEFT;
    if (m_Input.IsKeyHeld(GLFW_KEY_D))
        directions |= WalkState::MoveDirection::RIGHT;

    m_State.Update(dt, directions);
    m_Camera.SetPosition(m_State.GetPosition());
    UpdateCameraZoom(dt);
}

void WalkLayer::OnAttach() {
    m_Camera.SetZoom(INITIAL_ZOOM);
}

void WalkLayer::OnRender(Renderer2D& renderer) {
    const float playerSize = 100.0f;
    renderer.DrawQuad(
        m_State.GetPosition(),
        { playerSize, playerSize },
        m_MapPinTexture
    );
}

void WalkLayer::OnKey(int key, int action) {
    if (key == GLFW_KEY_0 && action == GLFW_PRESS) {
        m_Camera.SetZoom(INITIAL_ZOOM);
        m_State.Reset();
    }
}

void WalkLayer::UpdateCameraZoom(float dt) {
    if (m_Input.IsKeyHeld(GLFW_KEY_EQUAL) || m_Input.IsKeyHeld(GLFW_KEY_KP_ADD)) {
        float zoomDelta = m_ZoomSpeed * dt;
        float newZoom = m_Camera.GetZoom() + zoomDelta;
        m_Camera.SetZoom(glm::clamp(newZoom, m_MinZoom, m_MaxZoom));
    }
    if (m_Input.IsKeyHeld(GLFW_KEY_MINUS) || m_Input.IsKeyHeld(GLFW_KEY_KP_SUBTRACT)) {
        float zoomDelta = m_ZoomSpeed * dt;
        float newZoom = m_Camera.GetZoom() - zoomDelta;
        m_Camera.SetZoom(glm::clamp(newZoom, m_MinZoom, m_MaxZoom));
    }
}
