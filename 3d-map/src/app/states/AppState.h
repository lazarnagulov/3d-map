#pragma once
#include "WalkState.h"
#include "MeasureState.h"
#include "../layers/MeasureLayer.h"
#include "../layers/WalkLayer.h"

class AppState {
public:
    enum class Mode {
        WALK,
        MEASURE
    };

    AppState();
    using OnModeChangedCallback = std::function<void(Mode)>;

    void SwitchMode();
    void SetOnModeChanged(OnModeChangedCallback callback);
    inline Mode GetCurrentMode() const { return m_CurrentMode; }

private:
    Mode m_CurrentMode;
    OnModeChangedCallback m_OnModeChanged;
};