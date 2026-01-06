#pragma once
#include <glm/glm.hpp>

class WalkState {
public:
    enum MoveDirection : uint8_t {
        NONE = 0,
        UP = 1 << 0, 
        DOWN = 1 << 1,
        LEFT = 1 << 2, 
        RIGHT = 1 << 3 
    };

    WalkState(const glm::vec2& startPosition = { 0.0f, 0.0f });

    void Update(float deltaTime, uint8_t directions);
    void Reset();

    inline void SetPosition(const glm::vec2& pos) { m_Position = pos; }
    inline void SetMoveSpeed(float speed) { m_MoveSpeed = speed; }
    inline void SetBounds(const glm::vec2& min, const glm::vec2& max) {
        m_MinBounds = min;
        m_MaxBounds = max;
    }

    inline glm::vec2 GetPosition() const { return m_Position; }
    inline float GetWalkedDistance() const { return m_WalkedDistance; }
private:
    glm::vec2 m_Position;
    glm::vec2 m_Velocity{ 0.0f };
    glm::vec2 m_MinBounds{ 0,0 };
    glm::vec2 m_MaxBounds{ 0,0 };
    float m_WalkedDistance;
    float m_MoveSpeed;
};