#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "../../core/window/Camera3D.h"
#include "../../core/rendering/Renderer.h"

class MeasureState {
public:
    MeasureState() = default;

    void AddPoint(const glm::vec3& point);
    void RemovePoint(size_t index);
    void Reset();

    inline const std::vector<glm::vec3>& GetPoints() const { return m_Points; }
    inline float GetTotalDistance() const { return m_TotalDistance; }
    inline const std::vector<PointLight>& GetPinLights() const { return m_PinLights; }
    int FindPointNear(const glm::vec3& pos, float threshold = 10.0f) const;
private:
    void RecalculateDistance();
private:
    std::vector<glm::vec3> m_Points;
    std::vector<PointLight> m_PinLights;
    float m_TotalDistance = 0.0f;
};