#pragma once
#include "GUI_Element.h"
class GUI_Viewport :
    public GUI_Element
{
public:
    GUI_Viewport(const std::string& i_name, GUI_Interface* i_owner);
    void ReadIn(std::stringstream& i_stream)override;
    void OnClick(const sf::Vector2f& i_mousePos)override;
    void OnRelease()override;
    void OnHover(const sf::Vector2f& i_mousePos)override;
    void OnLeave()override;
    void Update(float i_dT)override;
    void Draw(sf::RenderTarget* i_target)override;
    void DrawOverlay(sf::RenderTarget* i_target) override;
    void ApplyBgStyle()override;

    sf::View& GetView();
    void SetViewSize(const sf::Vector2f& i_size);
    void SetViewCenter(const sf::Vector2f& i_pos);
	void SetBrush(std::string& i_texture, int i_tileId);

 
private:
    sf::View m_view;
    sf::Sprite m_hoverTile;
    sf::Vector2i m_hoverTilePos;
};

