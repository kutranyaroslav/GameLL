#include "GUI_Scrollbar.h"
#include <sstream>

GUI_Scrollbar::GUI_Scrollbar(const std::string& i_name, GUI_Interface* i_owner) :
	GUI_Element(i_name, GUI_ElementType::Scrollbar, i_owner) {
	m_isControl = true;
	debugText.setPosition(1400, 10);
	debugText.setFont(*i_owner->GetGuiManager()->GetSharedContext()->m_fontManager->GetResource("Main"));
}

GUI_Scrollbar::~GUI_Scrollbar(){}

void GUI_Scrollbar::ReadIn(std::stringstream& i_stream) {
	std::string type;
	i_stream >> type;
	if (type == "Horizontal") { 
		m_sliderType = SliderType::Horizontal; 
		m_slider.setPosition(0, GetPosition().y);
	}
	else { 
		m_sliderType = SliderType::Vertical; 
		m_slider.setPosition(GetPosition().x, 0);
	}
	
}
void GUI_Scrollbar::OnClick(const sf::Vector2f& i_mousePos) {
	if (!m_slider.getGlobalBounds().contains(i_mousePos - m_owner->GetPosition())) {
		return;
	}
	SetState(GUI_ElementState::Clicked);
	m_mouseMoveLast = i_mousePos;
}
void GUI_Scrollbar::OnRelease() {
	SetState(GUI_ElementState::Neutral);
}
void GUI_Scrollbar::OnHover(const sf::Vector2f& i_mousePos) {
	SetState(GUI_ElementState::Focused);
}
void GUI_Scrollbar::OnLeave() {
	SetState(GUI_ElementState::Neutral);
}
void GUI_Scrollbar::UpdateStyle(const GUI_ElementState& i_state, const GUI_Style& i_style) {
	GUI_Element::UpdateStyle(i_state, i_style);
	if (m_sliderType == SliderType::Horizontal) {
		m_styles[m_state].m_size.x = m_owner->GetSize().x;
	}
	else {
		m_styles[m_state].m_size.y = m_owner->GetSize().y;
	
	}
}
void GUI_Scrollbar::Update(float i_dT) {
	bool horizontal = m_sliderType == SliderType::Horizontal;
	if (GetState() != GUI_ElementState::Clicked) {
		return;
	}
	SharedContext* context = m_owner->GetGuiManager()->GetSharedContext();
	sf::Vector2f mousepos = sf::Vector2f(context->m_eventManager->GetMousePos(context->m_wind->GetRenderWindow()));
	if (m_mouseMoveLast == mousepos) { return; }
	sf::Vector2f difference = mousepos - m_mouseMoveLast;
	m_mouseMoveLast = mousepos;
	m_slider.move((horizontal ? sf::Vector2f(difference.x, 0.f) : sf::Vector2f(0.f, difference.y)));
	// check out of borders for slider rectshape
	if (horizontal && m_slider.getPosition().x < 0) {
		m_slider.setPosition(sf::Vector2f(0, m_slider.getPosition().y));
	}
	else if (m_slider.getPosition().y < 0) {
		m_slider.setPosition(sf::Vector2f(m_slider.getPosition().x, 0));
	}
	if (horizontal && (m_slider.getPosition().x + m_slider.getSize().x) > m_owner->GetSize().x) {
		m_slider.setPosition(m_owner->GetSize().x - m_slider.getSize().x, m_slider.getPosition().y);
	}
	else if(m_slider.getPosition().y + m_slider.getSize().y > m_owner->GetSize().y){
		m_slider.setPosition(m_slider.getPosition().x, m_owner->GetSize().y - m_slider.getSize().y);
	}
	else if (horizontal && m_owner->GetElement("ScrollbarVertical") && ((m_slider.getPosition().x + m_slider.getSize().x) >
		(m_owner->GetElement("ScrollbarVertical")->GetPosition().x) )){
		m_slider.setPosition(m_owner->GetElement("ScrollbarVertical")->GetPosition().x - m_slider.getSize().x, m_slider.getPosition().y);

	}
	float workArea = (horizontal ? m_owner->GetSize().x - m_slider.getSize().x
		: m_owner->GetSize().y - m_slider.getSize().y);
	int percentage = ((horizontal ? m_slider.getPosition().x : m_slider.getPosition().y) / m_workArea) * 100 ;
	if (horizontal) { m_owner->UpdateScrollHorizontal(percentage); }
	else { m_owner->UpdateScrollVertical(percentage); }
	SetRedraw(true);
}
void GUI_Scrollbar::Draw(sf::RenderTarget* i_target) {
	i_target->draw(debugText);
	i_target->draw(m_visual.m_backgroundSolid);
	i_target->draw(m_slider);
}
void GUI_Scrollbar::DrawOverlay(sf::RenderTarget* i_target)
{
}
void GUI_Scrollbar::ApplyStyle() {
	GUI_Element::ApplyStyle();
	m_slider.setFillColor(m_styles[m_state].m_elementColor);
	bool horizontal = m_sliderType == SliderType::Horizontal;
	auto& bgSolid = m_visual.m_backgroundSolid;
	SetPosition((horizontal ? sf::Vector2f(0, m_owner->GetSize().y - bgSolid.getSize().y): 
		sf::Vector2f(m_owner->GetSize().x - bgSolid.getSize().x ,0)));
	m_slider.setPosition((horizontal ? m_slider.getPosition().x : GetPosition().x),
		(horizontal ? GetPosition().y : m_slider.getPosition().y));
	float sizeFactor = (horizontal ? m_owner->GetContentSize().x / m_owner->GetSize().x :
		m_owner->GetContentSize().y / m_owner->GetSize().y);
	if (sizeFactor < 1.f) { sizeFactor = 1.f; }
	float sliderSize = (horizontal ? m_owner->GetSize().x : m_owner->GetSize().y) / sizeFactor;
	m_slider.setSize((horizontal ? sf::Vector2f(m_styles[m_state].m_elementSize.x, bgSolid.getSize().y) :
		sf::Vector2f(bgSolid.getSize().x, m_styles[m_state].m_elementSize.y)));
	bgSolid.setPosition(GetPosition());

	if (horizontal) {
		if (m_owner->GetElements().find("ScrollbarVertical") != m_owner->GetElements().end()) {
			bgSolid.setSize(sf::Vector2f(m_owner->GetSize().x - m_owner->GetElements().at("ScrollbarVertical")->GetSize().x, m_styles[m_state].m_size.y));
			m_workArea = bgSolid.getSize().x - m_slider.getSize().x;
		}

	}
	else if (!horizontal && m_owner->GetElements().find("ScrollbarHorizontal") != m_owner->GetElements().end()) {
		m_owner->GetElement("ScrollbarHorizontal")->SetSize(sf::Vector2f(m_owner->GetSize().x - m_styles[m_state].m_size.x, m_owner->GetElement("ScrollbarHorizontal")->GetSize().y));
		m_owner->GetElement("ScrollbarHorizontal")->SetWorkArea(m_owner->GetSize().x - m_styles[m_state].m_size.x);
	}
	else {
		bgSolid.setSize((horizontal ? sf::Vector2f(m_owner->GetSize().x, m_styles[m_state].m_size.y) :
			sf::Vector2f(m_styles[m_state].m_size.x, m_owner->GetSize().y)));
		m_workArea = bgSolid.getSize().y - m_slider.getSize().y;
	}

}

void GUI_Scrollbar::SetPosition(const sf::Vector2f& i_pos) {
	GUI_Element::SetPosition(i_pos);
	if (m_sliderType == SliderType::Horizontal) { m_position.x = 0; }
	else { m_position.y = 0; }

}

