#pragma once

#include "../../core/input/Input.h"
#include "../../core/layers/Layer.h"
#include "../../core/rendering/Renderer2D.h"
#include "../states/MeasureState.h"

class MeasureLayer : public Layer {
public:
	MeasureLayer(Input& input);

	void OnMouseButton(int button, int action, double x, double y) override;
	void OnKey(int key, int action) override;
	void OnRender(Renderer2D& renderer) override;
	inline void SetTextPosition(const glm::vec2& postion) { m_TextPosition = postion; }

private:
	void DrawLine(Renderer2D& renderer, const glm::vec2& p0, const glm::vec2& p1);
private:
	static constexpr float CIRCLE_RADIUS = 10.0f;
	static constexpr float LINE_THICKNESS = 5.0f;
	glm::vec2 m_TextPosition;
	Input& m_Input;
	MeasureState m_State;
};