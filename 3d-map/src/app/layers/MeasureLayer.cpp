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

    const Material PIN_CYLINDER_MAT{
        { 0.05f, 0.05f, 0.05f }, 
        { 0.15f, 0.15f, 0.15f },
        { 0.6f,  0.6f,  0.6f  },
        32.0f
    };

    const Material PIN_SPHERE_GLOW_MAT{
        { 0.4f,  0.0f,  0.0f },
        { 0.8f,  0.1f, 0.1f },
        { 1.0f,  0.3f,  0.3f  },
        64.0f
    };

    const Material LINE_CYLINDER_MAT{
        { 0.05f, 0.03f, 0.0f },
        { 0.2f,  0.12f, 0.0f },
        { 0.1f,  0.1f,  0.1f },
        8.0f
    };
}

glm::vec3 IntersectRayWithPlane(const Ray& ray, float planeY = 0.0f);

MeasureLayer::MeasureLayer(Input& input, Camera3D& camera)
    : m_Input(input), m_TextPosition({ 0.0f, 0.0f }), m_Camera(camera) {}

void MeasureLayer::OnMouseButton(int button, int action, double x, double y) {
    if (action != GLFW_PRESS)
        return;

    glm::vec3 mouseWorld = MouseToWorld(x, y);
    int index = m_State.FindPointNear(mouseWorld, 20.0f);
    
    if (index != -1) {
        m_State.RemovePoint(index);
        return;
    }

    m_State.AddPoint(mouseWorld);
}

void MeasureLayer::OnRender(Renderer& renderer) {
    InitializeMashes(renderer);
    const auto& points = m_State.GetPoints();

    std::vector<PointLight> lights;

    lights.push_back({
        { 0.0f, 10000.0f, 0.0f },
        glm::vec3(1.0f),
        1.0f
    });

    for (auto& l : m_State.GetPinLights())
        lights.push_back(l);

    renderer.UploadLights(lights);

    for (const auto& point : points) {
        glm::mat4 pinTransform = glm::translate(glm::mat4(1.0f), point);
        renderer.DrawMesh(*m_PinCylinder, pinTransform);
        glm::mat4 ballTransform = glm::translate(glm::mat4(1.0f), point + glm::vec3(0, 75, 0));
        renderer.DrawMesh(*m_PinSphere, ballTransform);
    }

    for (size_t i = 0; i + 1 < points.size(); i++) {
        glm::vec3 p1 = points[i] + glm::vec3(0, 0.5f, 0);
        glm::vec3 p2 = points[i + 1] + glm::vec3(0, 0.5f, 0);
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
        m_WindowWidth,
        m_WindowHeight
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
        m_PinCylinder->SetMaterial(PIN_CYLINDER_MAT);

        m_PinSphere = renderer.CreateSphereMesh(
            PIN_SPHERE_RADIUS,
            PIN_SPHERE_SEGMENTS
        );
        m_PinSphere->SetMaterial(PIN_SPHERE_GLOW_MAT);
    }

    if (!m_LineCylinder) {
        m_LineCylinder = renderer.CreateCylinderMesh(
            LINE_CYLINDER_RADIUS,
            LINE_CYLINDER_HEIGHT,
            LINE_CYLINDER_SEGMENTS
        );
        m_LineCylinder->SetMaterial(LINE_CYLINDER_MAT);
    }
}
static glm::vec3 IntersectRayWithPlane(const Ray& ray, float planeY) {
    float denom = ray.direction.y;

    if (glm::abs(denom) < 0.00001f)
        return ray.origin;

    float t = (planeY - ray.origin.y) / denom;
    return ray.origin + ray.direction * t;
}

