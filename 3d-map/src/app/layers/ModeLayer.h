#pragma once

#include "../../core/layers/Layer.h"
#include "../states/AppState.h"

class ModeLayer : public Layer {
public:
	ModeLayer(AppState& state);

	void OnRender(Renderer2D& renderer) override;
	void OnMouseButton(int button, int action, double x, double y) override;
private:
	AppState& m_State;
	Texture m_WalkingTexture;
	Texture m_MeasureTexture;

	glm::vec2 m_Position;
	glm::vec2 m_Size;
};
