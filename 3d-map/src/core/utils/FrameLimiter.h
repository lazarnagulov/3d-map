#pragma once

#include <GLFW/glfw3.h>

class FrameLimiter {
public:
    FrameLimiter(int targetFps);

    bool ShouldRender();
    double GetDeltaTime() const { return m_DeltaTime; }
    double GetFps() const {
        if (m_DeltaTime > 0.0)
            return 1.0 / m_DeltaTime;
        return 0.0;
    }

private:
    double m_TargetFrameTime;
    double m_LastFrameTime;
    double m_DeltaTime;
};
