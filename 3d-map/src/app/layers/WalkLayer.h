#pragma once

#include "../../core/layers/Layer.h"
#include "../../core/input/Input.h"
#include "../../core/rendering/Texture.h"
#include "../../core/rendering/Renderer.h"
#include "../../core/window/Camera2D.h"
#include "../states/WalkState.h"

class WalkLayer : public Layer {
public:
    WalkLayer(Input& input, Camera2D& camera);
    void OnUpdate(float dt) override; 
    void OnRender(Renderer& renderer) override;
    void OnKey(int key, int action) override;

    inline WalkState& GetState() { return m_State;  }
private:
    Input& m_Input;
    WalkState m_State;
    Camera2D& m_Camera;
    Texture m_MapPinTexture;

    std::unique_ptr<Mesh> m_PlayerCube;
    glm::vec3 m_Player3DPosition = { 0.0f, 25.0f, 0.0f };
    const float PLAYER_SIZE = 50.0f;
    const float PLAYER_HEIGHT = 25.0f;
};