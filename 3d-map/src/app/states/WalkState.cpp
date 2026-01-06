#include "WalkState.h"


WalkState::WalkState(const glm::vec2& startPosition /*= { 0.0f, 0.0f }*/)
	: m_Position(startPosition), m_WalkedDistance(0), m_MoveSpeed(200.0f)
{}

void WalkState::Update(float deltaTime, uint8_t directions) {
    glm::vec2 movement(0.0f);

    if (directions & UP)    movement.y += 1.0f;
    if (directions & DOWN)  movement.y -= 1.0f;
    if (directions & LEFT)  movement.x -= 1.0f;
    if (directions & RIGHT) movement.x += 1.0f;


    if (glm::length(movement) > 0.0f) {
        movement = glm::normalize(movement);
        glm::vec2 delta = movement * m_MoveSpeed * deltaTime;

        glm::vec2 newPos = m_Position + delta;

        newPos.x = glm::clamp(newPos.x, m_MinBounds.x, m_MaxBounds.x);
        newPos.y = glm::clamp(newPos.y, m_MinBounds.y, m_MaxBounds.y);


        if (newPos != m_Position) {
            m_WalkedDistance += glm::length(newPos - m_Position);
            m_Position = newPos;
        }
    }
}

void WalkState::Reset() {
	m_Position = glm::vec2(0.0f);
	m_WalkedDistance = 0.0f;
}
