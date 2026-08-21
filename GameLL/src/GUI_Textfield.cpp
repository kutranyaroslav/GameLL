#include "GUI_Textfield.h"
#include "GUI_Element.h"
GUI_Textfield::GUI_Textfield(const std::string& i_name, GUI_Interface* i_owner):
GUI_Element(i_name, GUI_ElementType::Textfield, i_owner){}

GUI_Textfield::~GUI_Textfield(){}

void GUI_Textfield::ReadIn(std::stringstream& i_stream) {
	std::string content; 
	Utils::ReadQuotedString(i_stream, content);
	m_visual.m_text.setString(content);
}

void GUI_Textfield::OnClick(const sf::Vector2f& i_mousePos) {
	SetState(GUI_ElementState::Clicked);
}
void GUI_Textfield::OnRelease() {}
void GUI_Textfield::OnHover(const sf::Vector2f& i_mousePos) {
	SetState(GUI_ElementState::Focused);
}
void GUI_Textfield::OnLeave() {
	SetState(GUI_ElementState::Neutral);
}
void GUI_Textfield::Update(float i_dT) {}
void GUI_Textfield::Draw(sf::RenderTarget* i_target) {
	i_target->draw(m_visual.m_backgroundSolid);
	if (m_styles[m_state].m_glyph != "") {
		i_target->draw(m_visual.m_glyph);
	}
	i_target->draw(m_visual.m_text);

}

void GUI_Textfield::DrawOverlay(sf::RenderTarget* i_target)
{
}

void GUI_Textfield::CallbackSetup()
{
}
