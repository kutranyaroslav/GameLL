#pragma once
#include "GUI_Manager.h"
#include "StateManager.h"

enum class SliderType{Horizontal, Vertical};
class GUI_Scrollbar :
    public GUI_Element
{
public:
    GUI_Scrollbar(const std::string& i_name, GUI_Interface* i_owner);
    ~GUI_Scrollbar();
	void ReadIn(std::stringstream& i_stream) override;
	void OnClick(const sf::Vector2f& i_mousePos) override;
	void OnRelease()override;
	void OnHover(const sf::Vector2f& i_mousePos)override;
	void OnLeave()override;
	void Update(float i_dT) override;	
	void Draw(sf::RenderTarget* i_target)override;
	void DrawOverlay(sf::RenderTarget* i_target)override;
	void UpdateStyle(const GUI_ElementState& i_state, const GUI_Style& i_style)override;
	void ApplyStyle() override;
	void SetPosition(const sf::Vector2f& i_pos) override;
	void CallbackSetup()override;
private: 
    SliderType m_sliderType;
    sf::Vector2f m_mouseMoveLast;
    int m_percentage;
	sf::Text debugText;
};

