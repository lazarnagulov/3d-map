#include "WalkLayer.h"
#include "GLFW/glfw3.h"

WalkLayer::WalkLayer(Input& input, Camera2D& camera) 
    : m_Input(input), m_Camera(camera), m_MapPinTexture("./src/assets/textures/map-pin.png") {}

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

    m_State.Update(dt, directions);
    m_Camera.SetPosition(m_State.GetPosition());
}

void WalkLayer::OnRender(Renderer& renderer) {
    glm::vec2 pos2D = m_State.GetPosition();
    glm::vec3 pos3D(pos2D.x, PLAYER_HEIGHT, pos2D.y);

    if (!m_PlayerCube) {
        m_PlayerCube = renderer.CreateCubeMesh(PLAYER_SIZE);
        m_PlayerCube->SetColor({ 0.2f, 0.6f, 1.0f, 1.0f });
    }

    glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos3D);
    renderer.DrawMesh(*m_PlayerCube, transform);
}

void WalkLayer::OnKey(int key, int action) {
    if (key == GLFW_KEY_0 && action == GLFW_PRESS) {
        m_State.Reset();
    }
}