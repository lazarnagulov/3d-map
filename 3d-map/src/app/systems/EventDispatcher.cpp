#include "EventDispatcher.h"

EventDispatcher::EventDispatcher(LayerStack& layerStack, Input& input)
    : m_LayerStack(layerStack), m_Input(input), m_WindowHeight(0), m_AppKeyHandler(nullptr)
{
}

void EventDispatcher::OnKey(int key, int action) {
    if (m_AppKeyHandler && m_AppKeyHandler(key, action)) 
        return;  

    DispatchToLayers([&](Layer& layer) {
        layer.OnKey(key, action);
    });
}

void EventDispatcher::OnMouseMove(double x, double y) {
    
    if (m_AppMouseMoveHandler && m_AppMouseMoveHandler(x, m_WindowHeight - y))
        return;
    
    DispatchToLayers([&](Layer& layer) {
        layer.OnMouseMove(x, m_WindowHeight - y);
    });
}

void EventDispatcher::OnMouseButton(int button, int action) {
    double x = m_Input.GetMouseX();
    double y = m_WindowHeight - m_Input.GetMouseY();

    if (m_AppMouseButtonHandler && m_AppMouseButtonHandler(button, action, x, y))
        return;

    DispatchToLayers([&](Layer& layer) {
        layer.OnMouseButton(button, action, x, y);
    });
}