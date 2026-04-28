#include "State_Developement.h"
#include "World.h"
#include "ErrorLogManager.h"
#include <fstream>
State_Developement::State_Developement(StateManager* i_stateManager)
	:BaseState(i_stateManager), m_layerIndex(0), m_currentTileset("")
{
}

void State_Developement::onCreate()
{

	EventManager* evMgr = m_stateManager->GetSharedContext()->m_eventManager;
	m_stateManager->GetSharedContext()->m_world->AddMap("MAP1", "TestTileset", "Tiles.cfg", "Tilesheet");
	m_stateManager->GetSharedContext()->m_world->AddTileset("MAP1", "TestTileset2", "Tiles.cfg", "Tilesheet2");
	m_stateManager->GetSharedContext()->m_world->LoadMap("MAP1");
	if (!m_stateManager->GetSharedContext()->m_world->GetCurrentMap()) { return; }
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
	gui->LoadInterface(StateType::Developement, "ListLayers.interface", "ListLayers");
	gui->LoadInterface(StateType::Developement, "ListLevels.interface", "ListLevels");
	//setting up levels interface
	float index0 = 0;
	float eLev_x = 0.f;
	float eLev_y = 0.f;
	float posLev_y = 0.f;
	for (auto itr = m_stateManager->GetSharedContext()->m_world->GetMaps().begin(); itr !=
		m_stateManager->GetSharedContext()->m_world->GetMaps().end(); itr++) {
		GUI_Interface* i = gui->GetInterface(StateType::Developement, "ListLevels");
		if (i) {
			i->AddElement(GUI_ElementType::Label, itr->first);
			GUI_Element* e = i->GetElement(itr->first);
			gui->LoadStyle("ListLevelsLabel.style", e);
			e->ApplyStyle();
			i->AdjustContentSize(e);
			eLev_x = e->GetPosition().x;
			eLev_y = e->GetPosition().y;
			posLev_y = eLev_y + i->GetPadding().y + (index0 * (e->GetSize().y + e->GetMargin().y));
			e->SetPosition(sf::Vector2f(eLev_x, posLev_y));
			e->SetText(itr->first);
			if(file.is_open()) {
				file << "\n" << itr->first << " " << "27:" << i->GetName() << ":" << e->GetName();
				evMgr->AddCallback(StateType::Developement, itr->first, &State_Developement::React, this);
			}
			index0++;
		}
	}

	//setting up layers interface
	float eL_x = 0.f;
	float eL_y = 0.f;
	float posL_y = 0.f;
	for (int i = 0; i < Sheet::Num_Layers; i++) {
		GUI_Interface* interf = gui->GetInterface(StateType::Developement, "ListLayers");
		if (interf) {
			interf->AddElement(GUI_ElementType::Label, "Layer_" + std::to_string(i));
			GUI_Element* e = interf->GetElement("Layer_" + std::to_string(i));
			gui->LoadStyle("ListLayersLabel.style", e);
			e->ApplyStyle();
			interf->AdjustContentSize(e);
			eL_x = e->GetPosition().x;
			eL_y = e->GetPosition().y;
			posL_y = eL_y + interf->GetPadding().y + (i * (e->GetSize().y + e->GetMargin().y));
			e->SetPosition(sf::Vector2f(eL_x, posL_y));
			e->SetText("Layer " + std::to_string(i));
			if (file.is_open()) {
				file << "\n" << "Layer_" << std::to_string(m_layerIndex) << " " << "27:" << interf->GetName()<< ":" << e->GetName();
				evMgr->AddCallback(StateType::Developement, "Key_" + std::to_string(m_layerIndex), &State_Developement::React, this);
				evMgr->AddCallback(StateType::Developement,"Layer_" + std::to_string(m_layerIndex), &State_Developement::React, this);
			}
			m_layerIndex++;
		}
	}



	//setting default map and getting default tileset 
	int index = 0;
	float e_x = 0.f;
	float e_y = 0.f;
	float pos_x = 0.f;
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
				file <<"\n" << itr->first << " " << "27:" << i->GetName() << ":"<<e->GetName();
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
	GUI_Manager* gui = m_stateManager->GetSharedContext()->m_guiManager;
	GUI_Interface* iMaps = gui->GetInterface(StateType::Developement, "ListLevels");
	GUI_Element* eMap = iMaps->GetElement(m_stateManager->GetSharedContext()->m_world->GetCurrentMap()->GetMapName());
	if (iMaps && eMap) {
		eMap->SetState(GUI_ElementState::Clicked);
	}
	GUI_Interface* iLayers = gui->GetInterface(StateType::Developement, "ListLayers");
	//reset all layers to neutral state and set the selected one to clicked state
	if (iLayers) {
		for (int i = 0; i < Sheet::Num_Layers; i++) {
			GUI_Element* e = iLayers->GetElement("Layer_" + std::to_string(i));
			if (e && !e->IsInside(
				sf::Vector2f(m_stateManager->GetSharedContext()->m_eventManager->GetMousePos(m_stateManager->GetSharedContext()->m_wind->GetRenderWindow()).x,
					m_stateManager->GetSharedContext()->m_eventManager->GetMousePos(m_stateManager->GetSharedContext()->m_wind->GetRenderWindow()).y )) ){
				e->SetState(GUI_ElementState::Neutral); // reset ALL
			}
		}

		GUI_Element* selected = iLayers->GetElement("Layer_" + std::to_string(m_layerIndex));
		if (selected) {
			selected->SetState(GUI_ElementState::Clicked); // set ONLY one

		}
	}
	GUI_Interface* iTilesets = gui->GetInterface(StateType::Developement, "ListBottom");
	if (m_currentTileset == "" || !iTilesets) { return; }
	GUI_Element* eTileset = iTilesets->GetElement(m_currentTileset);
	if (iTilesets && eTileset) {
		eTileset->SetState(GUI_ElementState::Clicked);
	}
}

void State_Developement::React(EventDetails* i_details)
{
	GUI_Manager* gui = m_stateManager->GetSharedContext()->m_guiManager;
	EventManager* evMgr = m_stateManager->GetSharedContext()->m_eventManager;
	if (m_stateManager->GetSharedContext()->m_world->HasMap(i_details->m_name) &&
		m_stateManager->GetSharedContext()->m_world->GetCurrentMap()->GetMapName() != i_details->m_name) {
		//clearing the current interface from elements
		for (auto itr = m_dynamicCallbacks.begin(); itr != m_dynamicCallbacks.end(); ++itr) {
			evMgr->RemoveCallback(m_state, *itr);
		}
		GUI_Interface* i = gui->GetInterface(StateType::Developement, "ListBottom");
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
			for (auto itr = m_dynamicCallbacks.begin(); itr != m_dynamicCallbacks.end(); ++itr) {
				if (key == *itr) { shouldRemove = true; break; };
			}
			if (!shouldRemove) {
				lines.push_back(line);
			}
		}
		file.close();
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
		if (i) {
			for (auto itr = m_stateManager->GetSharedContext()->m_world->GetCurrentMap()->GetTilesets().begin();
				itr != m_stateManager->GetSharedContext()->m_world->GetCurrentMap()->GetTilesets().end(); itr++) {
				GUI_Element* e = i->GetElement(itr->first);
				i->RemoveElement(e->GetName());
			}
		}
		outFile.close();
		//switching the map 
		m_stateManager->GetSharedContext()->m_world->SwitchTo(i_details->m_name);

		std::ofstream outFileNew;
		try {
			outFileNew.open(Utils::GetWorkingDirectory() + "//nav//binding_config.cfg", std::ios::app);
		}
		catch (const cException& e) {
			m_stateManager->GetSharedContext()->m_errorLogManager->LogException(e);
			THROW_EXCEPTION(2, "ERROR_FILE_NOT_FOUND");
		}
		int index = 0;
		float e_x = 0.f;
		float e_y = 0.f;
		float pos_x = 0.f;
		//placing new callbacks to out file and placing new elements
		for (auto itr = m_stateManager->GetSharedContext()->m_world->GetCurrentMap()->GetTilesets().begin();
			itr != m_stateManager->GetSharedContext()->m_world->GetCurrentMap()->GetTilesets().end(); itr++) {
			if (i) {
				i->AddElement(GUI_ElementType::Label, itr->first);
				GUI_Element* e = i->GetElement(itr->first);
				gui->LoadStyle("ListBottomLabel.style", e);
				e->ApplyStyle();
				i->AdjustContentSize(e);
				e_x = e->GetPosition().x;
				e_y = e->GetPosition().y;
				pos_x = e_x + i->GetPadding().x + (index * (e->GetSize().x + e->GetMargin().x));
				e->SetPosition(sf::Vector2f(pos_x, e_y));
				e->SetText(itr->first);
				++index;
				if (outFileNew.is_open()) {
					outFileNew << "\n" << itr->first << " " << "27:" << i->GetName() << ":" << e->GetName();
					evMgr->AddCallback(StateType::Developement, itr->first, &State_Developement::React, this);
					m_dynamicCallbacks.push_back(itr->first);
				}
			}

		}
		outFileNew.close();

	}
	for(auto itr = m_stateManager->GetSharedContext()->m_world->GetCurrentMap()->GetTilesets().begin();
		itr != m_stateManager->GetSharedContext()->m_world->GetCurrentMap()->GetTilesets().end(); itr++) {
		if (i_details->m_name == itr->first) {
			m_currentTileset = itr->first;
		}
	}
	for (int i = 0; i < Sheet::Num_Layers; i++) {
		if (i_details->m_name == "Layer_" + std::to_string(i) || i_details->m_name == "Key_" + std::to_string(i)) {
			m_layerIndex = i;
		}
		
	}

	if (i_details->m_name == "Key_Escape") {
		m_stateManager->SwitchTo(StateType::MainMenu);
	}

}
