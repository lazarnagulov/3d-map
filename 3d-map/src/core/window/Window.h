#pragma once

#include <GLFW/glfw3.h>
#include "../input/Input.h"
#include "IEventListener.h"

class Window {
public:
	Window(Input& input, IEventListener& listener);
	~Window();

	inline bool ShouldClose() const { return glfwWindowShouldClose(m_Window); };
	void ApplyLetterbox(float targetAspectRatio = 1.0f);
	void Update();
	void DisableSystemCursor();
	void Clear(float r, float g, float b, float a);

	inline int GetWidth() const { return m_Width; }
	inline int GetHeight() const { return m_Height; }
	inline GLFWwindow* GetWindow() const { return m_Window;  }
	inline Input& GetInput() const { return m_Input; }

private:
	void InitGLFW();
	void CreateFullscreenWindow();
	void InitGLEW();
	void SetupCallbacks();
private:
	int m_Width;
	int m_Height;
	GLFWwindow* m_Window;
	IEventListener& m_Listener;
	Input& m_Input;
};
