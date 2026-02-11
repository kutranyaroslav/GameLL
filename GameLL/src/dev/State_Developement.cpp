#include "State_Developement.h"

State_Developement::State_Developement(StateManager* i_stateManager)
	:BaseState(i_stateManager)
{
}

void State_Developement::onCreate()
{
	m_state = StateType::Developement;
	GUI_Manager* gui = m_stateManager->GetSharedContext()->m_guiManager;
	gui->LoadInterface(StateType::Developement, "ListBottomDevMode.interface", "ListBottom");

}

void State_Developement::onDestroy()
{
}

void State_Developement::Activate()
{
}

void State_Developement::Deactivate()
{
}

void State_Developement::Draw()
{
}

void State_Developement::Update(const sf::Time& i_time)
{
}
