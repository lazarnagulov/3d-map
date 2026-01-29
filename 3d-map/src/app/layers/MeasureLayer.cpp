#include "MeasureLayer.h"
#include "GLFW/glfw3.h"
#include <iostream>

namespace {
    constexpr float PIN_CYLINDER_RADIUS = 3.0f;
    constexpr float PIN_CYLINDER_HEIGHT = 75.0f;
    constexpr int   PIN_CYLINDER_SEGMENTS = 16;

    constexpr float PIN_SPHERE_RADIUS = 15.0f;
    constexpr int   PIN_SPHERE_SEGMENTS = 16;

    constexpr float LINE_CYLINDER_RADIUS = 2.0f;
    constexpr float LINE_CYLINDER_HEIGHT = 1.0f;
    constexpr int   LINE_CYLINDER_SEGMENTS = 12;

    constexpr glm::vec4 PIN_CYLINDER_COLOR{ 0.7f, 0.7f, 0.7f, 1.0f };
    constexpr glm::vec4 PIN_SPHERE_COLOR{ 1.0f, 0.0f, 0.0f, 1.0f };
    constexpr glm::vec4 LINE_CYLINDER_COLOR{ 1.0f, 0.5f, 0.0f, 1.0f };
}

glm::vec3 IntersectRayWithPlane(const Ray& ray, float planeY = 0.0f);

MeasureLayer::MeasureLayer(Input& input, Camera3D& camera)
    : m_Input(input), m_TextPosition({ 0.0f, 0.0f }), m_Camera(camera) {}

void MeasureLayer::OnMouseButton(int button, int action, double x, double y) {
    if (action != GLFW_PRESS)
        return;

    Ray ray = m_Camera.ScreenToWorld(
        static_cast<float>(x),
        static_cast<float>(y),
        m_WindowSize.x,
        m_WindowSize.y
    );

    const size_t pontCount = m_State.GetPoints().size();
    
    int index = m_State.FindPointNear(ray);
    if (index != -1 && index < pontCount) {
        m_State.RemovePoint(index);
        return;
    }

    glm::vec3 point = MouseToWorld(x, y);
    m_State.AddPoint(point);
}

void MeasureLayer::OnRender(Renderer& renderer) {
    InitializeMashes(renderer);
    const auto& points = m_State.GetPoints();

    for (const auto& point : points) {
        glm::mat4 pinTransform = glm::translate(glm::mat4(1.0f),point);
        renderer.DrawMesh(*m_PinCylinder, pinTransform);
        glm::mat4 ballTransform = glm::translate(glm::mat4(1.0f), glm::vec3(point.x, point.y + 75.0f, point.z));
        renderer.DrawMesh(*m_PinSphere, ballTransform);
    }
    for (size_t i = 0; i + 1 < points.size(); i++) {
        glm::vec3 p1(points[i].x, points[i].y + 0.5f , points[i].z);
        glm::vec3 p2(points[i + 1].x, points[i + 1].y + 0.5f, points[i + 1].z);
        DrawLine(renderer, p1, p2);
    }
}

void MeasureLayer::OnKey(int key, int action) {
    if (key == GLFW_KEY_0 && action == GLFW_PRESS)
        m_State.Reset();
}

glm::vec3 MeasureLayer::MouseToWorld(double x, double y) {
    Ray ray = m_Camera.ScreenToWorld(
        static_cast<float>(x),
        static_cast<float>(y),
        m_WindowSize.x,
        m_WindowSize.y
    );

    return IntersectRayWithPlane(ray, 0.0f);
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

void MeasureLayer::InitializeMashes(Renderer& renderer) {
    if (!m_PinCylinder) {
        m_PinCylinder = renderer.CreateCylinderMesh(
            PIN_CYLINDER_RADIUS,
            PIN_CYLINDER_HEIGHT,
            PIN_CYLINDER_SEGMENTS
        );
        m_PinCylinder->SetColor(PIN_CYLINDER_COLOR);

        m_PinSphere = renderer.CreateSphereMesh(
            PIN_SPHERE_RADIUS,
            PIN_SPHERE_SEGMENTS
        );
        m_PinSphere->SetColor(PIN_SPHERE_COLOR);
    }

    if (!m_LineCylinder) {
        m_LineCylinder = renderer.CreateCylinderMesh(
            LINE_CYLINDER_RADIUS,
            LINE_CYLINDER_HEIGHT,
            LINE_CYLINDER_SEGMENTS
        );
        m_LineCylinder->SetColor(LINE_CYLINDER_COLOR);
    }
}
static glm::vec3 IntersectRayWithPlane(const Ray& ray, float planeY) {
    float denom = ray.direction.y;

    if (glm::abs(denom) < 0.00001f)
        return ray.origin;

    float t = (planeY - ray.origin.y) / denom;
    return ray.origin + ray.direction * t;
}

