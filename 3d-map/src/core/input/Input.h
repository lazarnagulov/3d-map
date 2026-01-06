#pragma once

#include <bitset>

class Input {
public:
    void OnKeyEvent(int key, int action);
    void OnMouseButtonEvent(int button, int action);
    void OnMouseMove(double x, double y);

    inline bool IsKeyPressed(int key) const { return m_KeysPressed.test(key); } 
    inline bool IsKeyHeld(int key) const { return m_KeysHeld.test(key); }

    inline bool IsMouseButtonDown(int button) const { return m_MouseButtons.test(button); }

    inline double GetMouseX() const { return m_MouseX; }
    inline double GetMouseY() const { return m_MouseY; }

    void EndFrame(); 
private:
    static constexpr int MAX_KEYS = 512;
    static constexpr int MAX_MOUSE_BUTTONS = 8;
private:
    std::bitset<MAX_KEYS> m_KeysPressed{};
    std::bitset<MAX_KEYS> m_KeysHeld{};
    std::bitset<MAX_MOUSE_BUTTONS> m_MouseButtons{};
    double m_MouseX = 0, m_MouseY = 0;
};