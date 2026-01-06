#include "LayerStack.h"

void LayerStack::PushLayer(std::unique_ptr<Layer> layer) {
    layer->OnAttach();
    m_Layers.push_back(std::move(layer));
}

void LayerStack::PopLayer() {
    m_Layers.back()->OnDetach();
    m_Layers.pop_back();
}