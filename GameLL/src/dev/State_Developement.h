#pragma once
#include "../StateManager.h"
#include "../GUI_Manager.h"	
class State_Developement : public BaseState {
public:
	State_Developement(StateManager* i_stateManager);
	void onCreate()override;
	void onDestroy()override;
	void Activate()override;
	void Deactivate() override;
	void Draw()override;
	void Update(const sf::Time& i_time)override;
	void React(EventDetails* i_details);
	void OnResize(const sf::Vector2u& i_size) override;
	void SetUpLayoutTilesetBottom();
private:
	void PopulateTilesetDropdown();
	void SelectTileset(const std::string& i_tilesetName);
	void RemoveCurrentTilesetPreview();
	unsigned int m_layerIndex;
	int m_testIndex;
	std::string m_currentTileset;
	std::string m_tilesetPreviewName;
	bool m_hitboxEditMode = false;
};
