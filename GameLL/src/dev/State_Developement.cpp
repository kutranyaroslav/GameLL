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
			evMgr->AddDynamicBinding(itr->first, EventType::GUI_Click, i->GetName(), e->GetName());
			evMgr->AddCallback(StateType::Developement, itr->first, &State_Developement::React, this);
			m_dynamicCallbacks.push_back(itr->first);
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
			evMgr->AddDynamicBinding("Layer_" + std::to_string(m_layerIndex), EventType::GUI_Click, interf->GetName(), e->GetName());
			evMgr->AddCallback(StateType::Developement, "Key_" + std::to_string(m_layerIndex), &State_Developement::React, this);
			evMgr->AddCallback(StateType::Developement, "Layer_" + std::to_string(m_layerIndex), &State_Developement::React, this);
			m_dynamicCallbacks.push_back("Layer_" + std::to_string(m_layerIndex));
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
				e->SetWorld(m_stateManager->GetSharedContext()->m_world);
				i->AdjustContentSize(e);
				e_x = e->GetPosition().x;
				e_y = e->GetPosition().y;
				pos_x = e_x + i->GetPadding().x + (index * (e->GetSize().x + e->GetMargin().x));
				e->SetPosition(sf::Vector2f(pos_x, e_y));
				e->SetText(itr->first);
				++index;
				evMgr->AddDynamicBinding(itr->first, EventType::GUI_Click, i->GetName(), e->GetName());
				evMgr->AddCallback(StateType::Developement, itr->first, &State_Developement::React, this);
				m_dynamicCallbacks.push_back(itr->first);

			}
		}

		//setting up the viewport for the map 
		//TO DO doesn't work the size you have to add manually what is not good to be honest
		gui->LoadInterface(StateType::Developement, "ViewportDevMode.interface", "Viewport");
		GUI_Interface* i = gui->GetInterface(StateType::Developement, "Viewport");
		if (i) {
			//setting up the size of the viewport interface 
			float layersSizeX, mapsSizeX;
			layersSizeX = gui->GetInterface(StateType::Developement, "ListLayers")->GetSize().x;
			mapsSizeX = gui->GetInterface(StateType::Developement, "ListLevels")->GetSize().x;
			float listbottomSizeY;
			listbottomSizeY = gui->GetInterface(StateType::Developement, "ListBottom")->GetSize().y;
			if (layersSizeX > mapsSizeX) {
				i->SetSize(sf::Vector2f(m_stateManager->GetSharedContext()->m_wind->GetWindowSize().x - layersSizeX,
					m_stateManager->GetSharedContext()->m_wind->GetWindowSize().y - listbottomSizeY));
				i->ApplyStyle();
			}
			else {
				i->SetSize(sf::Vector2f(m_stateManager->GetSharedContext()->m_wind->GetWindowSize().x - mapsSizeX,
					m_stateManager->GetSharedContext()->m_wind->GetWindowSize().y - listbottomSizeY));
				i->ApplyStyle();
			}
			i->AddElement(GUI_ElementType::Viewport, "ViewportMap");
			GUI_Element* e = i->GetElement("ViewportMap");
			gui->LoadStyle("Viewport.style", e);
			e->ApplyStyle();
			//needed only for viewport
			e->SetWorld(m_stateManager->GetSharedContext()->m_world);
			e->SetTextureManager(m_stateManager->GetSharedContext()->m_textureManager);
			e->SetWindow(m_stateManager->GetSharedContext()->m_wind);
			e->SetZoomLevel();


			evMgr->AddDynamicBinding(e->GetName() + "Hover", EventType::GUI_Hover, i->GetName(), e->GetName());
			evMgr->AddCallback(StateType::Developement, e->GetName()+"Hover", &State_Developement::React, this);
			evMgr->AddCallback(StateType::Developement, "Key_Escape", &State_Developement::React, this);
		}
	
}
void State_Developement::onDestroy()
{
	EventManager* evMgr = m_stateManager->GetSharedContext()->m_eventManager;

	for (auto& name : m_dynamicCallbacks) {
		evMgr->RemoveBinding(name);
		evMgr->RemoveCallback(m_state, name);
	}
	m_dynamicCallbacks.clear();
	evMgr->RemoveBinding("Key_Escape");
	evMgr->RemoveBinding("ViewportMapHover");
	evMgr->RemoveCallback(StateType::Developement, "ViewportMap");
	evMgr->RemoveCallback(StateType::Developement, "Key_Escape");
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
	GUI_Manager* gui = m_stateManager->GetSharedContext()->m_guiManager;
	GUI_Interface* iMaps = gui->GetInterface(StateType::Developement, "ListLevels");
	GUI_Element* eMap = iMaps->GetElement(m_stateManager->GetSharedContext()->m_world->GetCurrentMap()->GetMapName());
	if (iMaps)
	{
		GUI_Element* eMap =
			iMaps->GetElement(m_stateManager->GetSharedContext()->m_world
				->GetCurrentMap()
				->GetMapName());
		if (eMap)
		{
			eMap->SetState(GUI_ElementState::Clicked);
		}
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
//to do finish the function get rid off this fucking file overwriting and just make it throuhg dynamic binding
void State_Developement::React(EventDetails* i_details)
{
	GUI_Manager* gui = m_stateManager->GetSharedContext()->m_guiManager;
	EventManager* evMgr = m_stateManager->GetSharedContext()->m_eventManager;
	if (m_stateManager->GetSharedContext()->m_world->HasMap(i_details->m_name) &&
		m_stateManager->GetSharedContext()->m_world->GetCurrentMap()->GetMapName() != i_details->m_name) {
		//clearing the hover tile of viewport
		GUI_Interface* iViewport = gui->GetInterface(StateType::Developement, "Viewport");


		//clearing the current interface from elements
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
				GUI_Element* e2 = i->GetElement(itr->first + "_Tileset");
				if (e) {
					i->RemoveElement(e->GetName());
				}
				if (e2) {
					i->RemoveElement(e2->GetName());
				}

			}
		}
		outFile.close();
		//switching the map 
		auto& dc = m_dynamicCallbacks;
		dc.erase(
			std::remove_if(dc.begin(), dc.end(), [&](const std::string& s) {
				// remove old tileset names (not Layer_ or Key_ entries)
				for (auto itr = m_stateManager->GetSharedContext()->m_world->GetCurrentMap()->GetTilesets().begin();
					itr != m_stateManager->GetSharedContext()->m_world->GetCurrentMap()->GetTilesets().end(); itr++)
					if (s == itr->first) return true;
				return false;
				}),
			dc.end()
		);
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
	//Clicks on GUI_Tileset buttons 
	for (auto itr = m_stateManager->GetSharedContext()->m_world->GetCurrentMap()->GetTilesets().begin();
		itr != m_stateManager->GetSharedContext()->m_world->GetCurrentMap()->GetTilesets().end(); itr++) {

		if (i_details->m_name == itr->first + "_Tileset") {
			GUI_Interface* i = gui->GetInterface(StateType::Developement, "ListBottom");
			auto* tileset = dynamic_cast<GUI_Tileset*>(i->GetElement(itr->first + "_Tileset"));
			GUI_Interface* v = gui->GetInterface(StateType::Developement, "Viewport");
			auto* viewport = dynamic_cast<GUI_Viewport*>(v->GetElement("ViewportMap"));
			if (tileset && viewport) {
				viewport->SetClickedTileInfo(tileset->GetSelectedTileInfo());
			}
		}

		if (i_details->m_name == itr->first) {
			GUI_Interface* i = gui->GetInterface(StateType::Developement, "ListBottom");
			if (i) {
				if (m_currentTileset != "" && m_currentTileset != itr->first) {
					GUI_Element* e = i->GetElement(m_currentTileset + "_Tileset");
					if (e) {
						i->RemoveElement(e->GetName());
					}

				}
			}
			m_currentTileset = itr->first;
			float i_y = 0.f;
			if (i) {
				i->AddElement(GUI_ElementType::Tileset, itr->first + "_Tileset");
				GUI_Element* e = i->GetElement(itr->first + "_Tileset");
				GUI_Element* e2 = i->GetElement(itr->first);
				i_y = e2->GetPosition().y + e2->GetSize().y + i->GetPadding().y;
				gui->LoadStyle("ListBottomTileset.style", e);
				e->SetBackgroundImage(itr->second.at(TileKey{ 0, 0 })->m_texture);
				e->SetPosition(sf::Vector2f(0, i_y));
				e->ApplyStyle();
				//setting up for the viewport
				e->SetWorld(m_stateManager->GetSharedContext()->m_world);
				i->AdjustContentSize(e);
				//setting up the Callbacks for the new tileset_Tileset element
				EventManager* evMgr = m_stateManager->GetSharedContext()->m_eventManager;
				evMgr->AddDynamicBinding(itr->first + "_Tileset", EventType::GUI_Click, i->GetName(), e->GetName());
				evMgr->AddCallback(StateType::Developement, itr->first + "_Tileset", &State_Developement::React, this);
			}

		}
	}



	for (int i = 0; i < Sheet::Num_Layers; i++) {
		if (i_details->m_name == "Layer_" + std::to_string(i) || i_details->m_name == "Key_" + std::to_string(i)) {
			m_layerIndex = i;
			GUI_Interface* i = gui->GetInterface(StateType::Developement, "Viewport");
			GUI_Element* e = i->GetElement("ViewportMap");
			e->SetLayer(m_layerIndex);
		}

	}

	if (i_details->m_name == "Key_Escape") {
		m_stateManager->SwitchTo(StateType::MainMenu);
	}
	if (i_details->m_name == "ViewportMapHover") {
		GUI_Interface* v = gui->GetInterface(StateType::Developement, "Viewport");
		GUI_Interface* t = gui->GetInterface(StateType::Developement, "ListBottom");
		GUI_Interface* m = gui->GetInterface(StateType::Developement, "ListLevels");
		if (v && t && m) {
			auto* tileset = dynamic_cast<GUI_Tileset*>(t->GetElement(m_currentTileset + "_Tileset"));
			auto* viewport = dynamic_cast<GUI_Viewport*>(v->GetElement("ViewportMap"));
			if (tileset && viewport) {
				viewport->SetBrush(tileset->GetTilesetTexture(), tileset->GetSelectedTileId());
			}
			if (!m_stateManager->GetSharedContext()->m_world->GetCurrentMap()->HasTileset(m_currentTileset)) {
				viewport->ClearBrush();
			}
		}

	}
}