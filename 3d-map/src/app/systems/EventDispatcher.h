#pragma once
#include "../../core/window/IEventListener.h"
#include "../../core/input/Input.h"
#include "../../core/layers/LayerStack.h"
#include <functional>

class EventDispatcher : public IEventListener {
public:
    explicit EventDispatcher(LayerStack& layerStack, Input& input);

    void OnKey(int key, int action) override;
    void OnMouseMove(double x, double y) override;
    void OnMouseButton(int button, int action) override;

    using KeyCallback = std::function<bool(int key, int action)>;
    using MouseMoveCallback = std::function<bool(double x, double y)>;
    using MouseButtonCallback = std::function<bool(int button, int action, double x, double y)>;

    inline void SetAppKeyHandler(KeyCallback handler) { m_AppKeyHandler = handler; }
    inline void SetMouseMoveHandler(MouseMoveCallback handler) { m_AppMouseMoveHandler = handler; }
    inline void SetMouseButtonHandler(MouseButtonCallback handler) { m_AppMouseButtonHandler = handler; }

    void SetWindowHeight(int height) { m_WindowHeight = height; }
    
    template<typename Event>
    void DispatchToLayers(Event&& eventCallback) {
        auto& layers = m_LayerStack.GetLayers();
        for (auto it = layers.rbegin(); it != layers.rend(); ++it) {
            if (!(*it)->IsEnabled())
                continue;
            eventCallback(**it);
        }
    }

private:
    LayerStack& m_LayerStack;
    Input& m_Input;
    int m_WindowHeight;

    KeyCallback m_AppKeyHandler;
    MouseMoveCallback m_AppMouseMoveHandler;
    MouseButtonCallback m_AppMouseButtonHandler;
};