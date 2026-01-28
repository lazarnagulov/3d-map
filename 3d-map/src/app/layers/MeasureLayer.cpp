#include "MeasureLayer.h"
#include "GLFW/glfw3.h"
#include <iostream>

MeasureLayer::MeasureLayer(Input& input) : m_Input(input), m_TextPosition({ 0.0f, 0.0f }) {}


void MeasureLayer::OnMouseButton(int button, int action, double x, double y) {
    glm::vec2 point = { static_cast<float>(x), static_cast<float>(y) };

    if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT) {
        int position = m_State.FindPointNear(point);
        position < 0 ? m_State.AddPoint(point) : m_State.RemovePoint(position);
    }
}

void MeasureLayer::OnRender(Renderer& renderer) {
    const auto& points = m_State.GetPoints();
    
    for (size_t i = 0; i < points.size(); i++) {
        const glm::vec2& p = points[i];

        renderer.DrawCircle(p, 10.0f, { 1,1,1,1 }, true);

        if (i > 0)
            DrawLine(renderer, points[i - 1], points[i]);
    }

    auto totalDistance = m_State.GetTotalDistance();
    renderer.DrawText(
        "Total distance: " + std::to_string(totalDistance), 
        m_TextPosition, 
        0.5f, 
        {0.0f, 0.0f, 0.0f, 1.0f}
    );
}

void MeasureLayer::DrawLine(Renderer& renderer, const glm::vec2& p0, const glm::vec2& p1) {
    glm::vec2 dir = p1 - p0;
    float len = glm::length(dir);

    if (len > 0.001f) {
        glm::vec2 n = dir / len;

        renderer.DrawLine(
            p0 + n * CIRCLE_RADIUS, 
            p1 - n * CIRCLE_RADIUS, 
            LINE_THICKNESS, 
            { 1.0f, 0.5f, 0.0f, 1.0f }
        );
    }
}

void MeasureLayer::OnKey(int key, int action) {
    if (key == GLFW_KEY_0 && action == GLFW_PRESS)
        m_State.Reset();
}