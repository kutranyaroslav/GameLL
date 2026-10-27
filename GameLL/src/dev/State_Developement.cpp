#include "State_Developement.h"
#include "World.h"
#include <algorithm>
#include <iostream>
State_Developement::State_Developement(StateManager* i_stateManager)
	:BaseState(i_stateManager), m_layerIndex(0), m_currentTileset(""), m_testIndex(0)
{
}

void State_Developement::onCreate()
{

	m_view = m_stateManager->GetSharedContext()->m_wind->GetUIView();
	EventManager* evMgr = m_stateManager->GetSharedContext()->m_eventManager;
	World* world = m_stateManager->GetSharedContext()->m_world;
	if (!world->HasMap("MAP1")) { world->AddMap("MAP1"); }
	if (!world->HasMap("MAP2")) { world->AddMap("MAP2"); }
	world->AddConfiguredTilesets("MAP1", true);
	world->AddConfiguredTilesets("MAP2", true);
	world->LoadMap("MAP2");
	if (!m_stateManager->GetSharedContext()->m_world->GetCurrentMap()) { return; }
	//open up the bind file to write the changes
	m_state = StateType::Developement;
	Binding* hitboxEditBinding = new Binding("ToggleHitboxEdit");
	hitboxEditBinding->BindEvent(EventType::KeyDown, EventInfo(sf::Keyboard::H));
	if (evMgr->AddBinding(hitboxEditBinding)) {
		evMgr->AddCallback(StateType::Developement, "ToggleHitboxEdit",
			&State_Developement::React, this);
		m_dynamicCallbacks.push_back("ToggleHitboxEdit");
	}
	else { delete hitboxEditBinding; }
	Binding* solidityBinding = new Binding("ToggleTileSolidity");
	solidityBinding->BindEvent(EventType::KeyDown, EventInfo(sf::Keyboard::B));
	if (evMgr->AddBinding(solidityBinding)) {
		evMgr->AddCallback(StateType::Developement, "ToggleTileSolidity",
			&State_Developement::React, this);
		m_dynamicCallbacks.push_back("ToggleTileSolidity");
	} else { delete solidityBinding; }
	GUI_Manager* gui = m_stateManager->GetSharedContext()->m_guiManager;

	gui->LoadInterface(StateType::Developement, "ListBottomDevMode.interface", "ListBottom");
	gui->LoadInterface(StateType::Developement, "ListLayers.interface", "ListLayers");
	gui->LoadInterface(StateType::Developement, "ListLevels.interface", "ListLevels");
	//setting up levels interface

	for (auto itr = m_stateManager->GetSharedContext()->m_world->GetMaps().begin(); itr !=
		m_stateManager->GetSharedContext()->m_world->GetMaps().end(); itr++) {
		GUI_Interface* i = gui->GetInterface(StateType::Developement, "ListLevels");
		if (i) {
			i->AddElement(GUI_ElementType::Label, itr->first);
			GUI_Element* e = i->GetElement(itr->first);
			gui->LoadStyle("ListLevelsLabel.style", e);
			e->ApplyStyle();
			i->AdjustContentSize(e);
			e->SetText(itr->first);
			evMgr->AddDynamicBinding(itr->first, EventType::GUI_Click, i->GetName(), e->GetName());
			evMgr->AddCallback(StateType::Developement, itr->first, &State_Developement::React, this);
			m_dynamicCallbacks.push_back(itr->first);
		}
	}

	//setting up layers interface
	for (int i = 0; i < Sheet::Num_Layers; i++) {
		GUI_Interface* interf = gui->GetInterface(StateType::Developement, "ListLayers");
		if (interf) {
			interf->AddElement(GUI_ElementType::Label, "Layer_" + std::to_string(i));
			GUI_Element* e = interf->GetElement("Layer_" + std::to_string(i));
			gui->LoadStyle("ListLayersLabel.style", e);
			e->ApplyStyle();
			interf->AdjustContentSize(e);
			e->SetText("Layer " + std::to_string(i));
			evMgr->AddDynamicBinding("Layer_" + std::to_string(i), EventType::GUI_Click, interf->GetName(), e->GetName());
			evMgr->AddCallback(StateType::Developement, "Key_" + std::to_string(i), &State_Developement::React, this);
			evMgr->AddCallback(StateType::Developement, "Layer_" + std::to_string(i), &State_Developement::React, this);
			m_dynamicCallbacks.push_back("Layer_" + std::to_string(i));
		}
	}
	// m_layerIndex is the selected layer, not a loop counter; layer 0 stays
	// selected until the user picks another.
	m_layerIndex = 0;


		GUI_Interface* tilesetPanel = gui->GetInterface(StateType::Developement, "ListBottom");
		if (tilesetPanel && tilesetPanel->AddElement(GUI_ElementType::Droplist, "TilesetDropdown")) {
			GUI_Element* element = tilesetPanel->GetElement("TilesetDropdown");
			gui->LoadStyle("ListBottomLabel.style", element);
			element->SetSize(sf::Vector2f(300.f, 40.f) * element->GetScale());
			element->SetPosition(sf::Vector2f(tilesetPanel->GetPadding().x,
				tilesetPanel->GetPadding().y));
			element->ApplyStyle();
			EventManager* events = m_stateManager->GetSharedContext()->m_eventManager;
			events->AddDynamicBinding("TilesetDropdown", EventType::GUI_Click,
				tilesetPanel->GetName(), element->GetName());
			events->AddCallback(StateType::Developement, "TilesetDropdown",
				&State_Developement::React, this);
			m_dynamicCallbacks.push_back("TilesetDropdown");
		}
		PopulateTilesetDropdown();
		SetUpLayoutTilesetBottom();

		//setting up the viewport for the map
		//TO DO doesn't work the size you have to add manually what is not good to be honest
		gui->LoadInterface(StateType::Developement, "ViewportDevMode.interface", "Viewport");
		GUI_Interface* i = gui->GetInterface(StateType::Developement, "Viewport");
		if (i) {
			// The viewport's size comes from ViewportInterface.style (85% x 65%,
			// i.e. the window minus the layers panel and the bottom panel). It used
			// to be computed here from the window size once, which meant it was
			// re-scaled on every later resize and drifted away from the window.
			i->ApplyStyle();
			i->AddElement(GUI_ElementType::Viewport, "ViewportMap");
			GUI_Element* e = i->GetElement("ViewportMap");
			GUI_Viewport* v = dynamic_cast<GUI_Viewport*>(e);
 			gui->LoadStyle("Viewport.style", e);
			e->ApplyStyle();
			//needed only for viewport
			e->SetWorld(m_stateManager->GetSharedContext()->m_world);
			e->SetTextureManager(m_stateManager->GetSharedContext()->m_textureManager);
			e->SetWindow(m_stateManager->GetSharedContext()->m_wind);
			e->SetZoomLevel();
			e->SetContext(m_stateManager->GetSharedContext());
			v->CallbackSetup();


			evMgr->AddDynamicBinding(e->GetName() + "Hover", EventType::GUI_Hover, i->GetName(), e->GetName());
			evMgr->AddCallback(StateType::Developement, e->GetName()+"Hover", &State_Developement::React, this);
			evMgr->AddCallback(StateType::Developement, "Key_Escape", &State_Developement::React, this);
		}
		// Interfaces and elements built above start at scale 1, while the rest of the
		// GUI is already scaled for the current window, so anything authored in
		// absolute pixels would stay at its 1920x1080 size until the first resize.
		// OnResize derives its delta from m_scale, so this is a no-op for interfaces
		// that are already at the current scale.
		gui->OnResize(m_stateManager->GetSharedContext()->m_wind->GetWindowSize());

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
	SharedContext* context = m_stateManager->GetSharedContext();
	if (!context || !context->m_guiManager || !context->m_world) { return; }
	GUI_Manager* gui = context->m_guiManager;
	GUI_Interface* iMaps = gui->GetInterface(StateType::Developement, "ListLevels");
	Map* currentMap = context->m_world->GetCurrentMap();
	if (iMaps && currentMap)
	{
		GUI_Element* eMap = iMaps->GetElement(currentMap->GetMapName());
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
}
void State_Developement::React(EventDetails* i_details)
{
	if (!i_details) { return; }
	SharedContext* context = m_stateManager->GetSharedContext();
	GUI_Manager* gui = context->m_guiManager;
	World* world = context->m_world;

	if (i_details->m_name == "TilesetDropdown") {
		GUI_Interface* panel = gui->GetInterface(StateType::Developement, "ListBottom");
		auto* dropdown = panel ? dynamic_cast<GUI_Droplist*>(panel->GetElement("TilesetDropdown")) : nullptr;
		if (dropdown) { SelectTileset(dropdown->GetSelectedOption()); }
		return;
	}
	if (i_details->m_name == "ToggleHitboxEdit") {
		m_hitboxEditMode = !m_hitboxEditMode;
		GUI_Interface* panel = gui->GetInterface(StateType::Developement, "ListBottom");
		auto* tileset = panel
			? dynamic_cast<GUI_Tileset*>(panel->GetElement(m_tilesetPreviewName)) : nullptr;
		if (tileset) { tileset->SetHitboxEditMode(m_hitboxEditMode); }
		return;
	}
	if (i_details->m_name == "ToggleTileSolidity") {
		GUI_Interface* panel = gui->GetInterface(StateType::Developement, "ListBottom");
		auto* tileset = panel
			? dynamic_cast<GUI_Tileset*>(panel->GetElement(m_tilesetPreviewName)) : nullptr;
		if (!tileset || !tileset->ToggleSelectedTileSolidity()) {
			std::cerr << "Select a tile in the tileset preview before toggling solidity.\n";
		} else if (TileInfo* selected = tileset->GetSelectedTileInfo()) {
			std::cout << "Tile solidity: " << (selected->m_solid ? "ON" : "OFF") << '\n';
		}
		return;
	}

	if (i_details->m_name == m_tilesetPreviewName && !m_tilesetPreviewName.empty()) {
		GUI_Interface* panel = gui->GetInterface(StateType::Developement, "ListBottom");
		GUI_Interface* viewportPanel = gui->GetInterface(StateType::Developement, "Viewport");
		auto* tileset = panel ? dynamic_cast<GUI_Tileset*>(panel->GetElement(m_tilesetPreviewName)) : nullptr;
		auto* viewport = viewportPanel ? dynamic_cast<GUI_Viewport*>(viewportPanel->GetElement("ViewportMap")) : nullptr;
		if (tileset && viewport) { viewport->SetClickedTileInfo(tileset->GetSelectedTileInfo()); }
		return;
	}

	if (world->HasMap(i_details->m_name) && world->GetCurrentMap() &&
		world->GetCurrentMap()->GetMapName() != i_details->m_name) {
		RemoveCurrentTilesetPreview();
		world->SwitchTo(i_details->m_name);
		PopulateTilesetDropdown();
		SetUpLayoutTilesetBottom();
		return;
	}

	for (int layer = 0; layer < Sheet::Num_Layers; ++layer) {
		if (i_details->m_name == "Layer_" + std::to_string(layer) ||
			i_details->m_name == "Key_" + std::to_string(layer)) {
			m_layerIndex = layer;
			GUI_Interface* viewportPanel = gui->GetInterface(StateType::Developement, "Viewport");
			GUI_Element* viewport = viewportPanel ? viewportPanel->GetElement("ViewportMap") : nullptr;
			if (viewport) { viewport->SetLayer(m_layerIndex); }
			return;
		}
	}

	if (i_details->m_name == "Key_Escape") {
		m_stateManager->SwitchTo(StateType::MainMenu);
		return;
	}
	if (i_details->m_name == "ViewportMapHover") {
		GUI_Interface* viewportPanel = gui->GetInterface(StateType::Developement, "Viewport");
		GUI_Interface* tilesetPanel = gui->GetInterface(StateType::Developement, "ListBottom");
		auto* tileset = tilesetPanel
			? dynamic_cast<GUI_Tileset*>(tilesetPanel->GetElement(m_tilesetPreviewName)) : nullptr;
		auto* viewport = viewportPanel
			? dynamic_cast<GUI_Viewport*>(viewportPanel->GetElement("ViewportMap")) : nullptr;
		if (tileset && viewport && tileset->GetSelectedTileId() >= 0) {
			viewport->SetBrush(tileset->GetTilesetTexture(), tileset->GetSelectedTileId());
		}
		else if (viewport) { viewport->ClearBrush(); }
	}
}

void State_Developement::PopulateTilesetDropdown()
{
	SharedContext* context = m_stateManager->GetSharedContext();
	GUI_Interface* panel = context->m_guiManager->GetInterface(StateType::Developement, "ListBottom");
	auto* dropdown = panel ? dynamic_cast<GUI_Droplist*>(panel->GetElement("TilesetDropdown")) : nullptr;
	if (!dropdown) { return; }
	dropdown->ClearOptions();
	Map* map = context->m_world->GetCurrentMap();
	if (!map) { RemoveCurrentTilesetPreview(); return; }
	for (const auto& tileset : map->GetTilesets()) { dropdown->AddOption(tileset.first); }
	if (dropdown->GetOptions().empty()) { RemoveCurrentTilesetPreview(); return; }
	dropdown->SetSelectedIndex(0);
	SelectTileset(dropdown->GetSelectedOption());
}

void State_Developement::SelectTileset(const std::string& i_tilesetName)
{
	if (i_tilesetName.empty()) { return; }
	SharedContext* context = m_stateManager->GetSharedContext();
	World* world = context->m_world;
	Map* map = world->GetCurrentMap();
	if (!map) { return; }
	const auto& allTilesets = map->GetTilesets();
	auto tilesetIt = allTilesets.find(i_tilesetName);
	if (tilesetIt == allTilesets.end()) { return; }
	if (m_currentTileset == i_tilesetName && !m_tilesetPreviewName.empty()) { return; }
	RemoveCurrentTilesetPreview();

	GUI_Manager* gui = context->m_guiManager;
	GUI_Interface* panel = gui->GetInterface(StateType::Developement, "ListBottom");
	if (!panel || !panel->AddElement(GUI_ElementType::Tileset, i_tilesetName + "_Tileset")) { return; }
	GUI_Element* element = panel->GetElement(i_tilesetName + "_Tileset");
	if (!element) { return; }
	gui->LoadStyle("ListBottomTileset.style", element);
	TileInfo* firstTile = nullptr;
	for (const auto& entry : tilesetIt->second) {
		if (entry.second) { firstTile = entry.second; break; }
	}
	if (!firstTile) { panel->RemoveElement(element->GetName()); return; }
	element->SetBackgroundImage(firstTile->m_texture);
	if (auto* tilesetPreview = dynamic_cast<GUI_Tileset*>(element)) {
		tilesetPreview->SetHitboxEditMode(m_hitboxEditMode);
	}
	GUI_Element* dropdown = panel->GetElement("TilesetDropdown");
	const float previewY = dropdown
		? dropdown->GetPosition().y + dropdown->GetSize().y + panel->GetPadding().y
		: panel->GetPadding().y;
	element->SetPosition(sf::Vector2f(panel->GetPadding().x, previewY));
	element->ApplyStyle();
	element->SetWorld(world);
	panel->AdjustContentSize(element);

	m_currentTileset = i_tilesetName;
	m_tilesetPreviewName = element->GetName();
	EventManager* events = context->m_eventManager;
	events->AddDynamicBinding(m_tilesetPreviewName, EventType::GUI_Click,
		panel->GetName(), m_tilesetPreviewName);
	events->AddCallback(StateType::Developement, m_tilesetPreviewName,
		&State_Developement::React, this);
	m_dynamicCallbacks.push_back(m_tilesetPreviewName);

	GUI_Interface* viewportPanel = gui->GetInterface(StateType::Developement, "Viewport");
	auto* viewport = viewportPanel
		? dynamic_cast<GUI_Viewport*>(viewportPanel->GetElement("ViewportMap")) : nullptr;
	if (viewport) {
		viewport->SetClickedTileInfo(nullptr);
		viewport->ClearBrush();
	}
}

void State_Developement::RemoveCurrentTilesetPreview()
{
	SharedContext* context = m_stateManager->GetSharedContext();
	if (m_tilesetPreviewName.empty()) { m_currentTileset.clear(); }
	else {
		context->m_eventManager->RemoveBinding(m_tilesetPreviewName);
		context->m_eventManager->RemoveCallback(StateType::Developement, m_tilesetPreviewName);
		m_dynamicCallbacks.erase(std::remove(m_dynamicCallbacks.begin(), m_dynamicCallbacks.end(),
			m_tilesetPreviewName), m_dynamicCallbacks.end());
		GUI_Interface* panel = context->m_guiManager->GetInterface(StateType::Developement, "ListBottom");
		if (panel) { panel->RemoveElement(m_tilesetPreviewName); }
		m_tilesetPreviewName.clear();
		m_currentTileset.clear();
	}
	GUI_Interface* viewportPanel = context->m_guiManager->GetInterface(StateType::Developement, "Viewport");
	auto* viewport = viewportPanel
		? dynamic_cast<GUI_Viewport*>(viewportPanel->GetElement("ViewportMap")) : nullptr;
	if (viewport) {
		viewport->SetClickedTileInfo(nullptr);
		viewport->ClearBrush();
	}
}

void State_Developement::OnResize(const sf::Vector2u& i_size)
{
	SetUpLayoutTilesetBottom();
}


void State_Developement::SetUpLayoutTilesetBottom()
{
	// Runs on every window resize, and can therefore fire before/while the dev-mode
	// interfaces are populated - every lookup below has to tolerate a missing
	// interface, element or map.
	SharedContext* context = m_stateManager->GetSharedContext();
	if (!context || !context->m_guiManager || !context->m_world) { return; }
	GUI_Manager* gui = context->m_guiManager;
	// Pin the tileset selector at the top of the panel and keep the sheet below it.
	GUI_Interface* i_ListBottom = gui->GetInterface(StateType::Developement, "ListBottom");
	if (i_ListBottom) {
		auto* dropdown = dynamic_cast<GUI_Droplist*>(i_ListBottom->GetElement("TilesetDropdown"));
		if (dropdown) {
			const float scale = dropdown->GetScale();
			const sf::Vector2f padding = i_ListBottom->GetPadding() * scale;
			const float centeredX = std::max(padding.x,
				(i_ListBottom->GetSize().x - dropdown->GetSize().x) * 0.5f);
			dropdown->SetPosition(sf::Vector2f(centeredX, padding.y));
			dropdown->SetRedraw(true);
			GUI_Element* preview = i_ListBottom->GetElement(m_tilesetPreviewName);
			if (preview) {
				preview->SetPosition(sf::Vector2f(padding.x,
					dropdown->GetPosition().y + dropdown->GetSize().y + padding.y));
				i_ListBottom->AdjustContentSize(preview);
				preview->SetRedraw(true);
			}
			i_ListBottom->SetRedraw(true);
		}
	}

	//setting default layout for maps/levels interface in dev mod
	GUI_Interface* i_ListMaps = gui->GetInterface(StateType::Developement, "ListLevels");
	if (i_ListMaps && !context->m_world->GetMaps().empty()) {
		GUI_Element* e_ListMaps = i_ListMaps->GetElement(context->m_world->GetMaps().begin()->first);
		if (e_ListMaps) {
			sf::Vector2f baseMaps = e_ListMaps->GetPosition();
			float index0 = 0;
			float posLev_y = 0.f;
			for (auto itr = context->m_world->GetMaps().begin(); itr != context->m_world->GetMaps().end(); itr++) {
				GUI_Element* e = i_ListMaps->GetElement(itr->first);
				if (!e) { continue; }
				posLev_y = baseMaps.y + (index0 * (e->GetSize().y + e->GetMargin().y));
				e->SetPosition(sf::Vector2f(baseMaps.x, posLev_y));
				index0++;
			}
			i_ListMaps->AdjustContentSize();
			i_ListMaps->SetRedraw(true);
		}
	}

	//setting default layout for layers interface in dev mod
	GUI_Interface* i_ListLayers = gui->GetInterface(StateType::Developement, "ListLayers");
	if (i_ListLayers) {
		GUI_Element* e_ListLayers = i_ListLayers->GetElement("Layer_0");
		if (e_ListLayers) {
			sf::Vector2f baseLayers = e_ListLayers->GetPosition();
			float posL_y = 0.f;
			for (int i = 0; i < Sheet::Num_Layers; i++) {
				GUI_Element* e = i_ListLayers->GetElement("Layer_" + std::to_string(i));
				if (!e) { continue; }
				posL_y = baseLayers.y + (i * (e->GetSize().y + e->GetMargin().y));
				e->SetPosition(sf::Vector2f(baseLayers.x, posL_y));
			}
			i_ListLayers->AdjustContentSize();
			i_ListLayers->SetRedraw(true);
		}
	}

	++m_testIndex;
}
