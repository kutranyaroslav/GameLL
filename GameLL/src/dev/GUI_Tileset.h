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

	void SetTileset(const std::string& i_tilesetName);
private:
    sf::Vector2f m_tilesetSize;
    int m_selectedTileId;
};

