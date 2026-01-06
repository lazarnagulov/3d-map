#pragma once

#include "../../core/layers/Layer.h"
#include "../../core/input/Input.h"
#include "../../core/rendering/Texture.h"
#include "../../core/rendering/Renderer2D.h"
#include "../../core/window/Camera.h"
#include "../states/WalkState.h"

class WalkLayer : public Layer {
public:
    WalkLayer(Input& input, Camera& camera);
    void OnUpdate(float dt) override; 
    void OnRender(Renderer2D& renderer) override;
    void OnKey(int key, int action) override;
    void OnAttach() override;

    inline WalkState& GetState() { return m_State;  }

private:
    void UpdateCameraZoom(float dt);
private:
    Input& m_Input;
    WalkState m_State;
    Camera& m_Camera;
    Texture m_MapPinTexture;


    float m_ZoomSpeed = 0.5f;
    float m_MinZoom = 1.0f;
    float m_MaxZoom = 2.5f; 
    static constexpr float INITIAL_ZOOM = 1.5f; 
};