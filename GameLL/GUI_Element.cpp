#include "GUI_Element.h"
#include "GUI_Manager.h"
GUI_Element::GUI_Element(const std::string& i_name, const GUI_ElementType& i_type, GUI_Interface* i_owner)
	:m_name(i_name), m_type(i_type), m_owner(i_owner), m_state(GUI_ElementState::Neutral),
	m_needsRedraw(false), m_active(true), m_isControl(false) {}

GUI_Element::~GUI_Element() { ReleaseResources(); }

void GUI_Element::SetName(const std::string& i_name) {
	m_name = i_name;
}
std::string& GUI_Element::GetName() { return m_name; }
void GUI_Element::SetOwner(GUI_Interface* i_owner) { m_owner = i_owner; }
GUI_Interface* GUI_Element::GetOwner() { return m_owner; }
void GUI_Element::SetRedraw(const bool& i_redraw) { m_needsRedraw = i_redraw; }
void GUI_Element::SetState(const GUI_ElementState& i_state) {
	if (i_state == m_state) { return; }
	m_state = i_state;
	SetRedraw(true);
}
GUI_ElementState& GUI_Element::GetState() { return m_state; }
GUI_ElementType& GUI_Element::GetType() { return m_type; }
void GUI_Element::SetPosition(const sf::Vector2f& i_pos) {
	m_position = i_pos;
	if (m_owner == nullptr) { return; }
	const auto& padding = m_owner->GetPadding();
	if (m_position.x < padding.x) { m_position.x = padding.x; }
	if (m_position.y < padding.y) { m_position.y = padding.y; }

}
sf::Vector2f& GUI_Element::GetPosition() { return m_position; }
sf::Vector2f GUI_Element::GetGlobalPosition() {
	sf::Vector2f position = GetPosition();
	if (m_owner == nullptr) { return position; }
	position += m_owner->GetGlobalPosition();
	if (IsControl()) { return position; }
	position.x -= m_owner->m_scrollHorizontal;
	position.y -= m_owner->m_scrollVertical;
	return position;
}

void GUI_Element::SetContentSize(sf::Vector2f& i_size) { m_styles.at(m_state).m_size = i_size; }
sf::Vector2f& GUI_Element::GetSize()
{
	return m_styles.at(m_state).m_size;
}
void GUI_Element::SetActive(const bool& const i_active)
{
	if (i_active != m_active) {
		m_active = i_active;
		SetRedraw(true);
	}
}
const bool& GUI_Element::GetActive()const { return m_active; }
void GUI_Element::SetText(const std::string& i_string) {
	m_visual.m_text.setString(i_string);
	SetRedraw(true);
}
bool GUI_Element::GerRedraw() { return m_needsRedraw; }
const std::string& GUI_Element::GetText()const { return m_visual.m_text.getString(); }
const bool GUI_Element::IsControl() { return m_isControl; }
bool GUI_Element::IsInside(const sf::Vector2f& i_point) {
	sf::Vector2f position = GetGlobalPosition();
	return(i_point.x >= position.x && i_point.y >= position.y &&
		i_point.x <= position.x + m_styles.at(m_state).m_size.x &&
		i_point.y <= position.y + m_styles.at(m_state).m_size.y);
}

void GUI_Element::ReleaseResources() {
	for (auto& itr : m_styles) {
		ReleaseTexture(itr.second.m_backgroundImage);
		ReleaseTexture(itr.second.m_glyph);
		ReleaseFont(itr.second.m_textFont);
	}
}
void GUI_Element::ReleaseTexture(const std::string& i_name) {
	if (i_name == "") { return; }
	m_owner->GetGuiManager()->GetSharedContext()->m_textureManager->ReleaseResource(i_name);
}

void GUI_Element::RequireTexture(const std::string& i_name) {
	if (i_name == "") { return; }
	m_owner->GetGuiManager()->GetSharedContext()->m_textureManager->RequireResource(i_name);
}

void GUI_Element::ReleaseFont(const std::string& i_name) {
	if (i_name == "") { return; }
	m_owner->GetGuiManager()->GetSharedContext()->m_fontManager->ReleaseResource(i_name);
}

void GUI_Element::RequireFont(const std::string& i_name) {
	if (i_name == "") { return; }
	m_owner->GetGuiManager()->GetSharedContext()->m_fontManager->RequireResource(i_name);
}


void GUI_Element::UpdateStyle(const GUI_ElementState& i_state, const GUI_Style& i_style) {
	if (i_style.m_backgroundImage != m_styles[i_state].m_backgroundImage) {
		ReleaseTexture(m_styles[i_state].m_backgroundImage);
		RequireTexture(i_style.m_backgroundImage);
	}
	if (i_style.m_glyph != m_styles[i_state].m_glyph) {
		ReleaseTexture(m_styles[i_state].m_glyph);
		RequireTexture(i_style.m_glyph);
	}
	if (i_style.m_textFont != m_styles[i_state].m_textFont) {
		ReleaseFont(m_styles[i_state].m_textFont);
		RequireFont(i_style.m_textFont);
	}
	m_styles[i_state] = i_style;
	if (i_state == m_state) { SetRedraw(true); ApplyStyle(); }
}


void GUI_Element::ApplyStyle() {
	ApplyTextStyle();
	ApplyGlyphStyle();
	ApplyBgStyle();
	if (IsControl()) {
		m_owner->AdjustContentSize();
	}
}

void GUI_Element::ApplyTextStyle() {
	FontManager* fonts = m_owner->GetGuiManager()->GetSharedContext()->m_fontManager;
	const GUI_Style& currentStyle = m_styles[m_state];
	if (currentStyle.m_textFont != "") {
		m_visual.m_text.setFont(*fonts->GetResource(currentStyle.m_textFont));
		m_visual.m_text.setFillColor(currentStyle.m_textColor);
		m_visual.m_text.setCharacterSize(currentStyle.m_textSize);
		if (currentStyle.m_textCenterOrigin) {
			sf::FloatRect rect = m_visual.m_text.getLocalBounds();
			m_visual.m_text.setOrigin(rect.left + rect.width / 2.0f, rect.top + rect.height / 2.0f);
		}
		else {
			m_visual.m_text.setOrigin(0.f, 0.f);
		}
	}
	m_visual.m_text.setPosition(m_position + currentStyle.m_textPadding);
}

void GUI_Element::ApplyBgStyle() {
	TextureManager* textureMgr = m_owner->GetGuiManager()->GetSharedContext()->m_textureManager;
	const GUI_Style& currentStyle = m_styles[m_state];
	if (currentStyle.m_backgroundImage != "") {
		m_visual.m_backgroundImage.setColor(currentStyle.m_backgroundImageColor);
		m_visual.m_backgroundImage.setTexture(*textureMgr->GetResource(currentStyle.m_backgroundImage));
		m_visual.m_backgroundImage.setPosition(m_position);

		m_visual.m_backgroundSolid.setSize(currentStyle.m_size);
		m_visual.m_backgroundSolid.setFillColor(currentStyle.m_backgroundColor);
		m_visual.m_backgroundSolid.setPosition(m_position);
	}
}

void GUI_Element::ApplyGlyphStyle() {
	TextureManager* textureMgr = m_owner->GetGuiManager()->GetSharedContext()->m_textureManager;
	const GUI_Style& currentStyle = m_styles[m_state];
	if (currentStyle.m_glyph != "") {
		m_visual.m_glyph.setTexture(*textureMgr->GetResource(currentStyle.m_glyph));
	}
	m_visual.m_glyph.setPosition(m_position + currentStyle.m_glyphPadding);

}