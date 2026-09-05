#include "GUI_Scrollbar.h"
#include <sstream>

GUI_Scrollbar::GUI_Scrollbar(const std::string& i_name, GUI_Interface* i_owner) :
	GUI_Element(i_name, GUI_ElementType::Scrollbar, i_owner) {
	m_isControl = true;
	debugText.setPosition(1400, 10);
	// GetResource returns null for a font that was never loaded.
	if (i_owner && i_owner->GetGuiManager() && i_owner->GetGuiManager()->GetSharedContext()->m_fontManager) {
		sf::Font* font = i_owner->GetGuiManager()->GetSharedContext()->m_fontManager->GetResource("Main");
		if (font) { debugText.setFont(*font); }
	}
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
	// Clamp the handle to its track along this bar's own axis. m_workArea is the
	// travel ApplyStyle worked out, so the track end needs no separate check and
	// there is no need to consult the perpendicular bar.
	sf::Vector2f handlePos = m_slider.getPosition();
	if (horizontal) {
		if (handlePos.x < 0.f) { handlePos.x = 0.f; }
		if (handlePos.x > m_workArea) { handlePos.x = m_workArea; }
	}
	else {
		if (handlePos.y < 0.f) { handlePos.y = 0.f; }
		if (handlePos.y > m_workArea) { handlePos.y = m_workArea; }
	}
	m_slider.setPosition(handlePos);
	if (m_workArea <= 0.f) { SetRedraw(true); return; }
	const float travel = (horizontal ? handlePos.x : handlePos.y);
	int percentage = static_cast<int>((travel / m_workArea) * 100.f);
	if (percentage < 0) { percentage = 0; }
	if (percentage > 100) { percentage = 100; }
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
	const bool horizontal = (m_sliderType == SliderType::Horizontal);
	auto& bgSolid = m_visual.m_backgroundSolid;
	const sf::Vector2f ownerSize = m_owner->GetSize();

	// Each bar sizes only itself. The thin axis comes from the style; the long axis
	// spans the owner less whatever the perpendicular bar occupies, so the two do
	// not overlap in the corner.
	const float thickness = (horizontal ? m_styles[m_state].m_size.y
		: m_styles[m_state].m_size.x);
	GUI_Element* crossBar = m_owner->GetElement(horizontal ? "ScrollbarVertical"
		: "ScrollbarHorizontal");
	float crossThickness = 0.f;
	if (crossBar) {
		crossThickness = (horizontal ? crossBar->GetSize().x : crossBar->GetSize().y);
	}
	float length = (horizontal ? ownerSize.x : ownerSize.y) - crossThickness;
	if (length < 0.f) { length = 0.f; }
	const sf::Vector2f barSize = (horizontal ? sf::Vector2f(length, thickness)
		: sf::Vector2f(thickness, length));
	bgSolid.setSize(barSize);
	SetSize(barSize);

	// Anchored to the owner's bottom edge (horizontal) or right edge (vertical).
	SetPosition(horizontal ? sf::Vector2f(0.f, ownerSize.y - barSize.y)
		: sf::Vector2f(ownerSize.x - barSize.x, 0.f));
	bgSolid.setPosition(GetPosition());

	// The handle covers the same fraction of the track as the visible area does of
	// the content, never smaller than the style's element size so it stays grabbable.
	const float content = (horizontal ? m_owner->GetContentSize().x
		: m_owner->GetContentSize().y);
	const float visible = (horizontal ? ownerSize.x : ownerSize.y);
	float fraction = 1.f;
	if (content > visible && content > 0.f) { fraction = visible / content; }
	const float minHandle = (horizontal ? m_styles[m_state].m_elementSize.x
		: m_styles[m_state].m_elementSize.y);
	float handleLength = length * fraction;
	if (handleLength < minHandle) { handleLength = minHandle; }
	if (handleLength > length) { handleLength = length; }
	m_slider.setSize(horizontal ? sf::Vector2f(handleLength, barSize.y)
		: sf::Vector2f(barSize.x, handleLength));

	// How far the handle can travel. Update() divides by this.
	m_workArea = length - handleLength;

	// Keep the handle on the bar's axis and inside the track after any resize.
	sf::Vector2f handlePos = m_slider.getPosition();
	if (horizontal) {
		handlePos.y = GetPosition().y;
		if (handlePos.x < 0.f) { handlePos.x = 0.f; }
		if (handlePos.x > m_workArea) { handlePos.x = m_workArea; }
	}
	else {
		handlePos.x = GetPosition().x;
		if (handlePos.y < 0.f) { handlePos.y = 0.f; }
		if (handlePos.y > m_workArea) { handlePos.y = m_workArea; }
	}
	m_slider.setPosition(handlePos);
}

void GUI_Scrollbar::SetPosition(const sf::Vector2f& i_pos) {
	GUI_Element::SetPosition(i_pos);
	if (m_sliderType == SliderType::Horizontal) { m_position.x = 0; }
	else { m_position.y = 0; }

}

void GUI_Scrollbar::CallbackSetup()
{
}

