#pragma once

#include "states/AppState.h"
#include "layers/MeasureLayer.h"
#include "layers/WalkLayer.h"
#include "layers/ModeLayer.h"
#include "layers/CompassCursorLayer.h"
#include "systems/LayerManager.h"
#include "systems/EventDispatcher.h"

#include "../core/window/Camera2D.h"
#include "../core/window/Camera3D.h"
#include "../core/window/Window.h"
#include "../core/layers/LayerStack.h"

class Application {
public:
	Application();
	~Application();

	void Run(float targetFps);
	void Update(float deltaTime);
	void Render();

	inline const Window& GetWindow() const { return m_Window; }
	inline Window& GetWindow() { return m_Window; }
private:
	void InitRenderer();
	void SyncLayersWithState();
	void PrepareFrame(int width, int height);
	void RenderWorld(int width, int height);
	void RenderUI(int width, int height);
private:
	Input m_Input;
	LayerStack m_LayerStack;
	LayerManager m_LayerManager;
	EventDispatcher m_EventDispatcher;
	Window m_Window;
	
	Camera2D m_Camera2D;
	Camera3D m_Camera3D;
	glm::vec2 m_CameraMoveDir;

	ModeLayer& m_ModeLayer;
	WalkLayer& m_WalkLayer;
	MeasureLayer& m_MeasureLayer;
	CompassCursorLayer& m_CursorLayer;

	AppState m_State;

	std::unique_ptr<Renderer> m_Renderer;
	std::shared_ptr<Shader> m_QuadShader;
	std::shared_ptr<Shader> m_TextShader;
	std::shared_ptr<Shader> m_MeshShader;

	std::shared_ptr<Texture> m_BackgroundTexture;
	std::unique_ptr<Mesh> m_MapMesh;
};
