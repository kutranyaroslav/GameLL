#include "StateManager.h"
#include "GUI_Manager.h"
#include "State_intro.h"
#include "State_MainMenu.h"
#include "State_Game.h"
#include "State_Paused.h"
#include "dev/State_Developement.h"
#include "GUI_Manager.h"
#include "Map.h"
#include "SoundManager.h"
#include "World.h"
StateManager::StateManager(SharedContext* i_shared):
	m_shared(i_shared)
{
	RegisterState<State_intro>(StateType::Intro);
	RegisterState<State_MainMenu>(StateType::MainMenu);
	RegisterState<State_Game>(StateType::Game);
	RegisterState<State_Paused>(StateType::Paused);
	//TO DO LATER IN GAME MODE THIS STATE MUST BE ERASED MANUALLY
	RegisterState<State_Developement>(StateType::Developement);
	/*RegisterState<State_GameOver>(StateType::GameOver);
	RegisterState<State_Credits>(StateType::Credits);*/
}

StateManager::~StateManager() {
	for (auto& itr : m_states) {
		itr.second->onDestroy();
		//potential bag maybe later one to itr assign nullptr
		delete itr.second;
	}
}

// The lowest state that still has to be drawn: the last one that paints its
// own background, since everything above it is transparent and draws on top.
StateContainer::iterator StateManager::FirstVisibleState() {
	auto itr = m_states.end();
	if (m_states.empty()) { return itr; }
	while (itr != m_states.begin()) {
		--itr;
		if (!itr->second->IsTransparent()) { break; }
	}
	return itr;
}

void StateManager::Draw() {
	if (m_states.empty()) { return; }
	for (auto itr = FirstVisibleState(); itr != m_states.end(); ++itr) {
		// A state that never set up a view keeps whatever the target has.
		if (itr->second->GetView()) {
			m_shared->m_wind->GetSceneTexture()->setView(*itr->second->GetView());
		}
		itr->second->Draw();
	}
}

void StateManager::DrawOverlay() {
	if (m_states.empty()) { return; }
	for (auto itr = FirstVisibleState(); itr != m_states.end(); ++itr) {
		itr->second->DrawOverlay();
	}
}

void StateManager::Update(const sf::Time& i_time) {
	if (m_states.empty()) { return; }
	if (m_states.back().second->IsTranscendent() && m_states.size() > 1) {
		auto itr = m_states.end();
		while (itr != m_states.begin()) {
			if (itr != m_states.end()) {
				if (itr->second->IsTranscendent()) {
					break;
				}
			}
			--itr;
		}
		for (;itr != m_states.begin(); ++itr) {
			itr->second->Update(i_time);
		}
	}
	else {
		m_states.back().second->Update(i_time);
	}

}

SharedContext* StateManager::GetSharedContext() { return m_shared; }

BaseState* StateManager::GetCurrentState()
{
	return m_currentState;
}

void StateManager::OnResize(const sf::Vector2u& i_size) {
	// Every live state is notified, not just the current one: GUI_Manager rescales
	// the interfaces of all states, so an inactive state would otherwise hold a
	// stale layout until it was switched to.
	for (auto itr = m_states.begin(); itr != m_states.end(); ++itr) {
		if (std::find(m_toRemove.begin(), m_toRemove.end(), itr->first) != m_toRemove.end()) { continue; }
		if (itr->second) { itr->second->OnResize(i_size); }
	}
}

bool StateManager::HasState(const StateType& i_type){
	for (auto itr = m_states.begin(); itr != m_states.end(); ++itr) {
		if (itr->first == i_type) {
			auto removed = std::find(m_toRemove.begin(), m_toRemove.end(), i_type);
			if (removed == m_toRemove.end()) { return true; }
			else { return false; }
		}
	}
	return false;
}

void StateManager::Remove(const StateType& i_type) {
	m_toRemove.push_back(i_type);
}

BaseState* StateManager::GetState(StateType i_type)
{
	auto itr = std::find_if(m_states.begin(), m_states.end(),
		[i_type](const std::pair<StateType, BaseState*>& s) { return s.first == i_type; });
	if (itr != m_states.end()) {
		return itr->second;
	}
	return nullptr;
}

void StateManager::ProcessRequests() {
	while (m_toRemove.begin() != m_toRemove.end()) {
		RemoveState(*m_toRemove.begin());
		m_toRemove.erase(m_toRemove.begin());
	}
}

void StateManager::SwitchTo(const StateType& i_type) {
	m_shared->m_eventManager->SetCurrentState(i_type);
	m_shared->m_guiManager->SetCurrentState(i_type);

	for (auto itr = m_states.begin(); itr != m_states.end(); itr++) {
		if (itr->first == i_type) {
			if (m_currentState) { m_currentState->Deactivate(); } // safe deactivate
			StateType tmp_type = itr->first;
			BaseState* tmp_state = itr->second;
			m_currentState = tmp_state;
			m_states.erase(itr);
			m_states.emplace_back(tmp_type, tmp_state);
			tmp_state->Activate();
			m_shared->m_soundManager->ChangeState(i_type); // fix bug 1
			return;
		}
	}

	if (!m_states.empty()) { m_states.back().second->Deactivate(); }
	CreateState(i_type);
	m_shared->m_soundManager->ChangeState(i_type);
	m_states.back().second->Activate();
}


void StateManager::CreateState(const StateType& i_type) {
	auto newState = m_stateFactory.find(i_type);
	if (newState == m_stateFactory.end()) { return; }
	BaseState* state = newState->second();
	m_states.emplace_back(i_type, state);
	m_currentState = m_states.back().second;
	state->onCreate();
}

void StateManager::RemoveState(const StateType& i_type) {
	for (auto itr = m_states.begin(); itr != m_states.end(); ++itr) {
		if (itr->first == i_type) {
			itr->second->onDestroy();
			delete itr->second;
			m_states.erase(itr);
			m_shared->m_soundManager->RemoveState(i_type);
			return;
		}
	}
}