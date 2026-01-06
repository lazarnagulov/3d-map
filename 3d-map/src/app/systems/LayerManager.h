#pragma once

#include "../../core/layers/LayerStack.h"
#include "../states/AppState.h"

class LayerManager {
public:
    LayerManager(LayerStack& stack) : m_LayerStack(stack) {}

    template<typename T, typename... Args>
    T& AddLayer(Args&&... args) {
        auto layer = std::make_unique<T>(std::forward<Args>(args)...);
        T& ref = *layer;
        m_LayerStack.PushLayer(std::move(layer));
        return ref;
    }

    LayerStack& GetLayerStack() { return m_LayerStack; }

private:
    LayerStack& m_LayerStack;
};