#pragma once

#include "../../core/input/Input.h"
#include "../../core/layers/Layer.h"
#include "../../core/rendering/Renderer.h"
#include "../../core/window/Camera3D.h"
#include "../states/MeasureState.h"

class MeasureLayer : public Layer {
public:
	MeasureLayer(Input& input, Camera3D& camera);
	void OnMouseButton(int button, int action, double x, double y) override;
	void OnMouseMove(double x, double y) override;
	void OnKey(int key, int action) override;
	void OnRender(Renderer& renderer) override;
	inline void SetTextPosition(const glm::vec2& postion) { m_TextPosition = postion; }
	inline void SetWindowSize(const glm::vec2& size) { m_WindowSize = size; }
	inline const MeasureState& GetState() const { return m_State; }
private:
	void DrawLine(Renderer& renderer, const glm::vec3& a, const glm::vec3& b);
	glm::vec2 ScreenToMap(double screenX, double screenY);
	void InitializeMashes(Renderer& renderer);
private:
	static constexpr float CIRCLE_RADIUS = 10.0f;
	static constexpr float LINE_THICKNESS = 5.0f;
	glm::vec2 m_TextPosition;
	glm::vec2 m_WindowSize;
	Input& m_Input;
	MeasureState m_State;
	Camera3D m_Camera;
	glm::vec2 m_HoverPoint { 0 };


	std::unique_ptr<Mesh> m_LineCylinder;
	std::unique_ptr<Mesh> m_HoverSphere;
	std::unique_ptr<Mesh> m_PinCylinder;
	std::unique_ptr<Mesh> m_PinSphere;
};