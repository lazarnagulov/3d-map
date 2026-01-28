#include "MeasureLayer.h"
#include "GLFW/glfw3.h"
#include <iostream>


MeasureLayer::MeasureLayer(Input& input, Camera3D& camera)
    : m_Input(input), m_TextPosition({ 0.0f, 0.0f }), m_Camera(camera) {}

void MeasureLayer::OnMouseButton(int button, int action, double x, double y) {
    glm::vec2 mapPoint = ScreenToMap(x, y);
    constexpr float MAP_HALF = 1000.0f;
    if (mapPoint.x < -MAP_HALF || mapPoint.x > MAP_HALF ||
        mapPoint.y < -MAP_HALF || mapPoint.y > MAP_HALF)
        return;
    m_State.AddPoint(mapPoint);
}

void MeasureLayer::OnRender(Renderer& renderer) {
    const auto& points = m_State.GetPoints();
    if (!m_PinCylinder) {
        m_PinCylinder = renderer.CreateCylinderMesh(3.0f, 50.0f, 16);
        m_PinCylinder->SetColor({ 0.7f, 0.7f, 0.7f, 1.0f });
        m_PinSphere = renderer.CreateSphereMesh(15.0f, 16);
        m_PinSphere->SetColor({ 1.0f, 0.0f, 0.0f, 1.0f });
    }
    for (const auto& point : points) {
        glm::mat4 pinTransform = glm::translate(glm::mat4(1.0f), glm::vec3(point.x, 0.0f, point.y));
        renderer.DrawMesh(*m_PinCylinder, pinTransform);
        glm::mat4 ballTransform = glm::translate(glm::mat4(1.0f), glm::vec3(point.x, 50.0f, point.y));
        renderer.DrawMesh(*m_PinSphere, ballTransform);
    }
    /* TODO: Draw lines
    for (size_t i = 0; i + 1 < points.size(); i++) {
        glm::vec3 p1(points[i].x, 0.5f, points[i].y);
        glm::vec3 p2(points[i + 1].x, 0.5f, points[i + 1].y);
    }
    */
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

glm::vec2 MeasureLayer::ScreenToMap(double screenX, double screenY) {
    Ray ray = m_Camera.ScreenToWorld(
        static_cast<float>(screenX),
        static_cast<float>(m_WindowSize.y - screenY),
        m_WindowSize.x,
        m_WindowSize.y
    );

    if (std::abs(ray.direction.y) < 0.0001f) {
        return glm::vec2(0.0f);
    }
    float t = -ray.origin.y / ray.direction.y;
    if (t < 0.0f) {
        return glm::vec2(0.0f);
    }
    glm::vec3 intersection = ray.origin + t * ray.direction;
    return glm::vec2(intersection.x, intersection.z);
}
