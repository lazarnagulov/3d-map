#pragma once

#pragma once
#include <vector>
#include <memory>
#include "Layer.h"

class LayerStack {
public:
    void PushLayer(std::unique_ptr<Layer> layer);
    void PopLayer();

    inline const std::vector<std::unique_ptr<Layer>>& GetLayers() const { return m_Layers; }

private:
    std::vector<std::unique_ptr<Layer>> m_Layers;
};