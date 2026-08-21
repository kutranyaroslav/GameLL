#include "GUI_Label.h"


GUI_Label::GUI_Label(const std::string & i_name, GUI_Interface* i_owner): 
GUI_Element(i_name, GUI_ElementType::Label, i_owner){}

void GUI_Label::ReadIn(std::stringstream& i_stream) {
	std::string content;
	Utils::ReadQuotedString(i_stream, content);
	m_visual.m_text.setString(content);
}
void GUI_Label::OnClick(const sf::Vector2f& i_mousePos) {
	SetState(GUI_ElementState::Clicked);
}
void GUI_Label::OnRelease(){
	SetState(GUI_ElementState::Neutral);
}
void GUI_Label::OnHover(const sf::Vector2f& i_mousePos) {
	SetState(GUI_ElementState::Focused);
}
void GUI_Label::OnLeave() {
	SetState(GUI_ElementState::Neutral);
}
void GUI_Label::Update(float i_dT) {}
void GUI_Label::Draw(sf::RenderTarget* i_target) {
	i_target->draw(m_visual.m_backgroundSolid);
	if (m_styles[m_state].m_glyph != "") {
		i_target->draw(m_visual.m_glyph);
	}
	i_target->draw(m_visual.m_text);
	i_target->draw(m_visual.m_backgroundImage);
}

void GUI_Label::DrawOverlay(sf::RenderTarget* i_target)
{
}

void GUI_Label::CallbackSetup()
{
}
