#include "CompassCursorLayer.h"


CompassCursorLayer::CompassCursorLayer(Input& input, const glm::vec2& target)
    : m_CompassBase("./src/assets/textures/compass.png"),
      m_Input(input),
      m_Target(target) {}

void CompassCursorLayer::OnRender(Renderer& renderer) {
    float mouseX = m_Input.GetMouseX();
    float mouseY = m_Target.y - m_Input.GetMouseY();

    glm::vec2 cursorPos = { mouseX, mouseY };
    glm::vec2 compassSize = { 48.0f, 48.0f };
    renderer.DrawQuad(cursorPos, compassSize, m_CompassBase);

    glm::vec2 dir = glm::normalize(m_Target - cursorPos);

    float needleLength = 20.0f;
    glm::vec2 needleEnd = cursorPos + dir * needleLength;

    renderer.DrawLine(cursorPos, needleEnd, 3.0f, { 1.0f, 0.0f, 0.0f, 1.0f });
}

