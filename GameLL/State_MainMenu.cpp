#include "State_MainMenu.h"
#include "GUI_Manager.h"

State_MainMenu::State_MainMenu(StateManager* i_stateManager) :
	BaseState(i_stateManager) {}

void State_MainMenu::onCreate() {
	GUI_Manager* guiMgr = m_stateManager->GetSharedContext()->m_guiManager;
	Window* wind = m_stateManager->GetSharedContext()->m_wind;
	guiMgr->LoadInterface(StateType::MainMenu, "MainMenu.interface", "MainMenu");
	EventManager* evMgr = m_stateManager->GetSharedContext()->m_eventManager;
	evMgr->AddCallback(StateType::MainMenu,"MainMenu_Play", &State_MainMenu::Play, this);
	evMgr->AddCallback(StateType::MainMenu, "MainMenu_Quit",&State_MainMenu::Quit, this);
}

void State_MainMenu::onDestroy() {
	EventManager* evMgr = m_stateManager->GetSharedContext()->m_eventManager;
	GUI_Manager* guiMgr = m_stateManager->GetSharedContext()->m_guiManager;
	evMgr->RemoveCallback(StateType::MainMenu, "MainMenu_Play");
	evMgr->RemoveCallback(StateType::MainMenu, "MainMenu_Quit");
	//potential memory leak cause we create interface but destroy it in gui_manager 

}

void State_MainMenu::Activate() {
	auto play = m_stateManager->GetSharedContext()->m_guiManager->GetInterface(StateType::MainMenu, "MainMenu")
		->GetElement("Play");
	if (!play) {
		return;
	}
	if (m_stateManager->HasState(StateType::Game)) {
		play->SetText("Resume");
	}
	else {
		play->SetText("Play");
	}
}

void State_MainMenu::Deactivate(){}

void State_MainMenu::MouseClick(EventDetails* i_details) {

}

void State_MainMenu::Play(EventDetails* i_details)
{
	m_stateManager->SwitchTo(StateType::Game);
}

void State_MainMenu::Quit(EventDetails* i_details)
{
	m_stateManager->GetSharedContext()->m_wind->Close();
}

void State_MainMenu::Update(const sf::Time& i_time){}

void State_MainMenu::Draw() {
	
}