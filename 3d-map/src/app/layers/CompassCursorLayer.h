#pragma once

#include "../../core/layers/Layer.h"
#include "../../core/input/Input.h"

class CompassCursorLayer : public Layer {
public:
    CompassCursorLayer(Input& input, const glm::vec2& target);

    void OnRender(Renderer& renderer) override;
private:
    Input& m_Input;
    Texture m_CompassBase;
    glm::vec2 m_Target;
};
