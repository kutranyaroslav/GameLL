#pragma once
#include "GUI_Element.h"
class GUI_Label :
    public GUI_Element
{
public:
    GUI_Label(const std::string& i_name, GUI_Interface* i_owner);

    void ReadIn(std::stringstream& i_stream)override;
    void OnClick(const sf::Vector2f& i_mousePos)override;
    void OnRelease()override;
    void OnHover(const sf::Vector2f& i_mousePos)override;
    void OnLeave()override;
    void Update(float i_dT)override;
    void Draw(sf::RenderTarget* i_target)override;
    void DrawOverlay(sf::RenderTarget* i_target)override;
};

