#include "MeasureState.h"

void MeasureState::AddPoint(const glm::vec3& point) {
	m_Points.push_back(point);
	m_PinLights.push_back({
		point + glm::vec3(0, 75, 0),
		glm::vec3(1, 0, 0),
		2.0f
	});
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

int MeasureState::FindPointNear(const glm::vec3& pos, float threshold /* = 10.0f */) const {
	for (size_t i = 0; i < m_Points.size(); ++i) {
		float dist = glm::distance(m_Points[i], pos);
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
