#include "FrameLimiter.h"

FrameLimiter::FrameLimiter(int targetFps)
    : m_TargetFrameTime(1.0 / targetFps)
    , m_LastFrameTime(glfwGetTime())
    , m_DeltaTime(0.0) {}

bool FrameLimiter::ShouldRender() {
    double currentTime = glfwGetTime();
    m_DeltaTime = currentTime - m_LastFrameTime;

    if (m_DeltaTime >= m_TargetFrameTime) {
        m_LastFrameTime = currentTime;
        return true;
    }

    return false;
}