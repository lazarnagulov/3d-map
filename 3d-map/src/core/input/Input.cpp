#include "Input.h"
#include <GLFW/glfw3.h>

void Input::OnKeyEvent(int key, int action) {
    if (action == GLFW_PRESS) {
        m_KeysPressed.set(key);
        m_KeysHeld.set(key);  
    }
    else if (action == GLFW_RELEASE) {
        m_KeysHeld.reset(key);
    }
}
void Input::OnMouseButtonEvent(int button, int action) {
    if (action == GLFW_PRESS)
        m_MouseButtons.set(button);
    else if (action == GLFW_RELEASE)
        m_MouseButtons.set(button);
}

void Input::OnMouseMove(double x, double y) {
    m_MouseX = x;
    m_MouseY = y;
}

void Input::EndFrame() {
    m_KeysPressed.reset();
}

