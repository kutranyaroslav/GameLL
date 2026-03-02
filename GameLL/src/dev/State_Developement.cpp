#include "State_Developement.h"
#include "World.h"
State_Developement::State_Developement(StateManager* i_stateManager)
	:BaseState(i_stateManager)
{
}

void State_Developement::onCreate()
{
	m_state = StateType::Developement;
	GUI_Manager* gui = m_stateManager->GetSharedContext()->m_guiManager;
	gui->LoadInterface(StateType::Developement, "ListBottomDevMode.interface", "ListBottom");
	EventManager* evMgr = m_stateManager->GetSharedContext()->m_eventManager;
	//setting default map and getting default tileset 
	int index = 0;
	float e_x = 0.f; 
	float e_y = 0.f;
	float pos_x = 0.f;
	float pos_y = 0.f;
	for (auto itr = m_stateManager->GetSharedContext()->m_world->GetCurrentMap()->GetTilesets().begin();
		itr != m_stateManager->GetSharedContext()->m_world->GetCurrentMap()->GetTilesets().end(); ++itr) {
		GUI_Interface* i = gui->GetInterface(StateType::Developement, "ListBottom");
		if (i) {
			i->AddElement(GUI_ElementType::Label, itr->first);
			GUI_Element* e = i->GetElement(itr->first);
			gui->LoadStyle("ListBottomLabel.style", e);
			e_x = e->GetPosition().x;
			e_y = e->GetPosition().y;
			pos_x = e_x + i->GetPadding().x  + (index * (e->GetSize().x  + e->GetMargin().x));
			e->SetPosition(sf::Vector2f(pos_x, e_y));
			e->SetText(itr->first);
			++index;
			evMgr->AddCallback(StateType::Developement, itr->first, &State_Developement::React, this);
		}
	}
	

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

void State_Developement::React(EventDetails* i_details)
{
	float s_x, s_y; 
	GUI_Interface* i = m_stateManager->GetSharedContext()->m_guiManager->GetInterface(StateType::Developement, "ListBottom");
	s_x,s_y = i->GetSize().x, i->GetSize().y;

}
