#pragma once
#include "../GUI_Element.h"
#include "../Map.h"
class GUI_Tileset :
    public GUI_Element
{
public:
    GUI_Tileset(const std::string& i_name, GUI_Interface* i_owner);
    void ReadIn(std::stringstream& i_stream)override;
    void OnClick(const sf::Vector2f& i_mousePos)override;
    void OnRelease()override;
    void OnHover(const sf::Vector2f& i_mousePos)override;
    void OnLeave()override;
    void Update(float i_dT)override;
    void Draw(sf::RenderTarget* i_target)override;
    void DrawOverlay(sf::RenderTarget* i_target) override;

    void ApplyBgStyle() override;
    void CallbackSetup()override;
	void SetTileset(const std::string& i_tilesetName);
	void SetHitboxEditMode(bool i_enabled);
	bool GetHitboxEditMode() const;
	bool ToggleSelectedTileSolidity();

	std::string&  GetTilesetTexture();
	// One tile as it appears on screen (texture tile * current GUI scale).
	float GetScaledTileSize();
	int GetSelectedTileId() const;
    //needed for the viewport to get the info of the selected tile 
	TileInfo* GetSelectedTileInfo();
private:
    sf::RectangleShape m_hoverTile;
    sf::RectangleShape m_selectedTile;
    sf::RectangleShape m_hitboxOutline;
    sf::Color m_clickedTileDefColor; 
    sf::Color m_hoverTileDefColor;
    sf::Vector2i m_clickedTilePos{ -1, -1 };
    sf::Vector2i m_hoverTilePos{ -1,-1 };
    sf::Vector2f m_tilesetSize;
    int m_selectedTileId;
	bool m_hitboxEditMode = false;
	bool m_draggingHitbox = false;
	sf::Vector2f m_hitboxDragStart{ 0.f, 0.f };
	sf::Vector2f m_hitboxDragEnd{ 0.f, 0.f };
	void UpdateHitboxFromMouse(const sf::Vector2f& i_mousePos);
};

