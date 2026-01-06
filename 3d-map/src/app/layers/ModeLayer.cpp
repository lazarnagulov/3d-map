#include "ModeLayer.h"
#include "GLFW/glfw3.h"

ModeLayer::ModeLayer(AppState& state) 
	: m_State(state), 
	  m_WalkingTexture("./src/assets/textures/walk-mode.png"),
      m_MeasureTexture("./src/assets/textures/measure-mode.png"),
	  m_Position({ 150.0f, 150.0f }),
	  m_Size({ 200.0f, 200.0f }) {}

void ModeLayer::OnRender(Renderer2D& renderer) {
	renderer.DrawQuad(
		m_Position,
		m_Size,
		m_State.GetCurrentMode() == AppState::Mode::MEASURE ? m_MeasureTexture : m_WalkingTexture
	);
}

void ModeLayer::OnMouseButton(int button, int action, double x, double y) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        float halfW = m_Size.x * 0.5f;
        float halfH = m_Size.y * 0.5f;

        bool inside =
            x >= (m_Position.x - halfW) &&
            x <= (m_Position.x + halfW) &&
            y >= (m_Position.y - halfH) &&
            y <= (m_Position.y + halfH);

        if (inside) 
            m_State.SwitchMode();
    }
}