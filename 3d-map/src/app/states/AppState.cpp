#include "AppState.h"

AppState::AppState() : m_CurrentMode(Mode::WALK) {}

void AppState::SwitchMode() {
	m_CurrentMode = (m_CurrentMode == Mode::WALK) ? Mode::MEASURE : Mode::WALK;

	if (m_OnModeChanged) 
		m_OnModeChanged(m_CurrentMode);
}

void AppState::SetOnModeChanged(OnModeChangedCallback callback) {
	m_OnModeChanged = std::move(callback);
}