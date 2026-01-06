#pragma once

#include "../rendering/Renderer2D.h"

class Layer {
public:
    virtual ~Layer() = default;

    virtual void OnAttach() {}      
    virtual void OnDetach() {}     

    virtual void OnUpdate(float deltaTime) {} 
    virtual void OnRender(Renderer2D& renderer) {}       

    virtual void OnKey(int key, int action) {}
    virtual void OnMouseButton(int button, int action, double x, double y) {}
    virtual void OnMouseMove(double x, double y) {}

    void SetEnabled(bool enabled) { m_Enabled = enabled; }
    bool IsEnabled() const { return m_Enabled; }

protected:
    bool m_Enabled = true;
};