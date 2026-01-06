#pragma once
#include <vector>
#include <glm/glm.hpp>

class MeasureState {
public:
    MeasureState() = default;

    void AddPoint(const glm::vec2& point);
    void RemovePoint(size_t index);
    void Reset();

    inline const std::vector<glm::vec2>& GetPoints() const { return m_Points; }
    inline float GetTotalDistance() const { return m_TotalDistance; }

    int FindPointNear(const glm::vec2& pos, float threshold = 10.0f) const;

private:
    void RecalculateDistance();
private:
    std::vector<glm::vec2> m_Points;
    float m_TotalDistance = 0.0f;
};