#include "MeasureLayer.h"
#include "GLFW/glfw3.h"
#include <iostream>


MeasureLayer::MeasureLayer(Input& input, Camera3D& camera)
    : m_Input(input), m_TextPosition({ 0.0f, 0.0f }), m_Camera(camera) {}

void MeasureLayer::OnMouseButton(int button, int action, double x, double y) {
    m_State.AddPoint(m_HoverPoint);
}

void MeasureLayer::OnMouseMove(double x, double y) {
    m_HoverPoint = ScreenToMap(x, m_WindowSize.y  - y);
}

void MeasureLayer::InitializeMashes(Renderer& renderer) {
    if (!m_HoverSphere) {
        m_HoverSphere = renderer.CreateSphereMesh(10.0f, 16);
        m_HoverSphere->SetColor({ 1.0f, 1.0f, 0.0f, 0.5f });
    }

    if (!m_PinCylinder) {
        m_PinCylinder = renderer.CreateCylinderMesh(3.0f, 75.0f, 16);
        m_PinCylinder->SetColor({ 0.7f, 0.7f, 0.7f, 1.0f });
        m_PinSphere = renderer.CreateSphereMesh(15.0f, 16);
        m_PinSphere->SetColor({ 1.0f, 0.0f, 0.0f, 1.0f });
    }

    if (!m_LineCylinder) {
        m_LineCylinder = renderer.CreateCylinderMesh(2.0f, 1.0f, 12);
        m_LineCylinder->SetColor({ 1.0f, 0.5f, 0.0f, 1.0f });
    }
}

void MeasureLayer::OnRender(Renderer& renderer) {
    InitializeMashes(renderer);
    glm::mat4 hoverTransform = glm::translate(glm::mat4(1.0f),
    glm::vec3(m_HoverPoint.x, 25.0f, m_HoverPoint.y));
    renderer.DrawMesh(*m_HoverSphere, hoverTransform);
    const auto& points = m_State.GetPoints();

    for (const auto& point : points) {
        glm::mat4 pinTransform = glm::translate(glm::mat4(1.0f), glm::vec3(point.x, 0.0f, point.y));
        renderer.DrawMesh(*m_PinCylinder, pinTransform);
        glm::mat4 ballTransform = glm::translate(glm::mat4(1.0f), glm::vec3(point.x, 75.0f, point.y));
        renderer.DrawMesh(*m_PinSphere, ballTransform);
    }
    for (size_t i = 0; i + 1 < points.size(); i++) {
        glm::vec3 p1(points[i].x, 0.5f, points[i].y);
        glm::vec3 p2(points[i + 1].x, 0.5f, points[i + 1].y);
        DrawLine(renderer, p1, p2);
    }
}

void MeasureLayer::DrawLine(Renderer& renderer, const glm::vec3& p0, const glm::vec3& p1) {
    glm::vec3 dir = p1 - p0;
    float len = glm::length(dir);
    if (len < 0.001f)
        return;

    glm::vec3 n = glm::normalize(dir);

    glm::vec3 up(0.0f, 1.0f, 0.0f);
    glm::vec3 axis = glm::cross(up, n);
    float axisLen = glm::length(axis);

    glm::mat4 rot(1.0f);
    if (axisLen > 0.0001f) {
        axis /= axisLen;
        float angle = acos(glm::clamp(glm::dot(up, n), -1.0f, 1.0f));
        rot = glm::rotate(glm::mat4(1.0f), angle, axis);
    }
    else if (glm::dot(up, n) < 0.0f) {
        rot = glm::rotate(glm::mat4(1.0f), glm::pi<float>(), glm::vec3(1, 0, 0));
    }

    glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), { 1.0f, len, 1.0f });
    glm::mat4 translateUp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, 0.0f));

    glm::mat4 transform = glm::translate(glm::mat4(1.0f), p0)
        * rot
        * translateUp
        * scaleMat;

    renderer.DrawMesh(*m_LineCylinder, transform);
}

void MeasureLayer::OnKey(int key, int action) {
    if (key == GLFW_KEY_0 && action == GLFW_PRESS)
        m_State.Reset();
}

glm::vec2 MeasureLayer::ScreenToMap(double x, double y) {
    float u = x / m_WindowSize.x;
    float v = y / m_WindowSize.y;

    constexpr float MAP_SIZE = 2000.0f;
    float mapX = (u - 0.5f) * MAP_SIZE;
    float mapZ = (v - 0.5f) * MAP_SIZE;

    return { mapX, mapZ };
}
