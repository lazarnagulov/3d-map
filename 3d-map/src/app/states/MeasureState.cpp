#include "MeasureState.h"

void MeasureState::AddPoint(const glm::vec3& point) {
	m_Points.push_back(point);
	RecalculateDistance();
}

void MeasureState::RemovePoint(size_t index) {
	if (index < m_Points.size()) {
		m_Points.erase(m_Points.begin() + index);
		RecalculateDistance();
	}
}

void MeasureState::Reset() {
	m_Points.clear();
	m_TotalDistance = 0.0f;
}

int MeasureState::FindPointNear(const Ray& ray, float threshold) const {
    for (size_t i = 0; i < m_Points.size(); ++i) {
        const glm::vec3& p = m_Points[i];

        glm::vec3 toPoint = p - ray.origin;
        float t = glm::dot(toPoint, ray.direction);

        if (t < 0.0f)
            continue;

        glm::vec3 closest = ray.origin + ray.direction * t;
        float dist = glm::distance(p, closest);

        if (dist <= threshold)
            return static_cast<int>(i);
    }

    return -1;
}


void MeasureState::RecalculateDistance() {
	m_TotalDistance = 0.0f;

	for (size_t i = 1; i < m_Points.size(); ++i) 
		m_TotalDistance += glm::distance(m_Points[i - 1], m_Points[i]);
}
