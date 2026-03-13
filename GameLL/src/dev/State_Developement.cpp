#include "State_Developement.h"
#include "World.h"
#include "ErrorLogManager.h"
#include <fstream>
State_Developement::State_Developement(StateManager* i_stateManager)
	:BaseState(i_stateManager)
{
}

void State_Developement::onCreate()
{
	//open up the bind file to write the changes 
	std::ofstream file;
	try {
		file.open(Utils::GetWorkingDirectory() + "//nav//binding_config.cfg", std::ios::app);
	}
	catch (const cException& e) {
		m_stateManager->GetSharedContext()->m_errorLogManager->LogException(e);
		THROW_EXCEPTION(2, "ERROR_FILE_NOT_FOUND");
	}
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
			e->ApplyStyle();
			i->AdjustContentSize(e);
			e_x = e->GetPosition().x;
			e_y = e->GetPosition().y;
			pos_x = e_x + i->GetPadding().x  + (index * (e->GetSize().x  + e->GetMargin().x));
			e->SetPosition(sf::Vector2f(pos_x, e_y));
			e->SetText(itr->first);
			++index;
			if (file.is_open()) {
				file <<"\n" << itr->first << " " << "27:" << i->GetName() << ":React";
				evMgr->AddCallback(StateType::Developement, itr->first, &State_Developement::React, this);
				m_dynamicCallbacks.push_back(itr->first);
			}
		}
	}
	file.close();

	evMgr->AddCallback(StateType::Developement, "Key_Escape", &State_Developement::React, this);

}

void State_Developement::onDestroy()
{
	EventManager* evMgr = m_stateManager->GetSharedContext()->m_eventManager;
	for (auto itr = m_dynamicCallbacks.begin(); itr != m_dynamicCallbacks.end(); ++itr){
		evMgr->RemoveCallback(m_state, *itr);
	}
	//open the file to clean up dynamica callbacks which had been adde to the binding_config.cfg 
	std::ifstream file;
	try {
		file.open(Utils::GetWorkingDirectory() + "//nav//binding_config.cfg", std::ios::in | std::ios::out);
	}
	catch (const cException& e) {
		m_stateManager->GetSharedContext()->m_errorLogManager->LogException(e);
		THROW_EXCEPTION(2, "ERROR_FILE_NOT_FOUND");
	}
	std::string line;
	std::vector<std::string> lines;

	while (std::getline(file, line)) {
		std::stringstream ss(line);
		std::string key;
		ss >> key;
		bool shouldRemove = false;
		for (auto itr = m_dynamicCallbacks.begin(); itr != m_dynamicCallbacks.end(); ++itr){
			if (key == *itr) { shouldRemove = true; break; };
		}
		if (!shouldRemove) {
			lines.push_back(line);
		}
	}
	file.close();
	//needs to be tested if the file correctly being overwritten with all the bindings except the one dynamic 
	std::ofstream outFile;

	try {
		outFile.open(Utils::GetWorkingDirectory() + "//nav//binding_config.cfg", std::ios::trunc);
	}
	catch (const cException& e) {
		m_stateManager->GetSharedContext()->m_errorLogManager->LogException(e);
		THROW_EXCEPTION(2, "ERROR_FILE_NOT_FOUND");
	}

	// Write the remaining lines back to the file
	for (const auto& validLine : lines) {
		outFile << validLine << std::endl;
	}

	outFile.close();
	evMgr->RemoveCallback(StateType::Developement,"Key_Escape");
}


void State_Developement::Activate()
{
}

void State_Developement::Deactivate()
{
}

void State_Developement::Draw()
{
	for (unsigned int i = 0; i < Sheet::Num_Layers; ++i) {
		m_stateManager->GetSharedContext()->m_world->Draw(i);
	}
}

void State_Developement::Update(const sf::Time& i_time)
{
}

void State_Developement::React(EventDetails* i_details)
{
	float s_x, s_y; 
	GUI_Interface* i = m_stateManager->GetSharedContext()->m_guiManager->GetInterface(StateType::Developement, "ListBottom");
	s_x= i->GetSize().x;
	s_y = i->GetSize().y;
	if (i_details->m_name == "Key_Escape") {
		m_stateManager->SwitchTo(StateType::MainMenu);
	}

}
