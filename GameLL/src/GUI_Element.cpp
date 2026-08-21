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
sf::Vector2f& GUI_Element::GetMargin() { return m_styles[m_state].m_margin; }
sf::Vector2f GUI_Element::GetGlobalPosition() {
	sf::Vector2f position = GetPosition();
	if (m_owner == nullptr) { return position; }
	position += m_owner->GetGlobalPosition();
	if (IsControl()) { return position; }
	position.x -= m_owner->m_scrollHorizontal;
	position.y -= m_owner->m_scrollVertical;
	return position;
}

sf::RectangleShape& GUI_Element::GetSlider()
{
	return m_slider;
}

void GUI_Element::SetContentSize(sf::Vector2f& i_size) { m_styles.at(m_state).m_size = i_size; }
void GUI_Element::SetSize(const sf::Vector2f& i_size)
{
	m_styles[GUI_ElementState::Clicked].m_size = i_size;
	m_styles[GUI_ElementState::Focused].m_size = i_size;
	m_styles[GUI_ElementState::Neutral].m_size = i_size;
}
void GUI_Element::SetWorkArea(float i_area)
{
	m_workArea = i_area;
}
sf::Vector2f& GUI_Element::GetSize()
{
	return m_styles.at(m_state).m_size;
}
sf::Vector2i GUI_Element::GetBgImageSize()
{
	return m_visual.m_backgroundImage.getTextureRect().getSize();
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
void GUI_Element::SetBackgroundImage(const std::string& i_texture)
{	
	m_styles[m_state].m_backgroundImage = i_texture;
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
	if (i_name == "" || m_owner == nullptr) { return; }
	m_owner->GetGuiManager()->GetSharedContext()->m_textureManager->ReleaseResource(i_name);
}

void GUI_Element::RequireTexture(const std::string& i_name) {
	if (i_name == "" || m_owner == nullptr) { return; }
	m_owner->GetGuiManager()->GetSharedContext()->m_textureManager->RequireResource(i_name);
}

void GUI_Element::ReleaseFont(const std::string& i_name) {
	if (i_name == "" || m_owner == nullptr) { return; }
	m_owner->GetGuiManager()->GetSharedContext()->m_fontManager->ReleaseResource(i_name);
}

void GUI_Element::RequireFont(const std::string& i_name) {
	if (i_name == "" || m_owner == nullptr) { return; }
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
	ApplyBgStyle();
	ApplyTextStyle();
	ApplyGlyphStyle();
	
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
			sf::FloatRect rectBg = m_visual.m_backgroundSolid.getLocalBounds();
			m_visual.m_text.setOrigin(rect.left + rect.width/2.f, rect.top + rect.height/2.f);
			m_visual.m_text.setStyle(sf::Text::Bold);
			m_visual.m_text.setPosition(m_position.x + m_styles[m_state].m_size.x/2.f, m_position.y + m_styles[m_state].m_size.y/2.f);
			
		}
		else {
			m_visual.m_text.setOrigin(0.f, 0.f);
			m_visual.m_text.setPosition(m_position + currentStyle.m_textPadding);
		}
	}
	
}

void GUI_Element::ApplyBgStyle() {
	TextureManager* textureMgr = m_owner->GetGuiManager()->GetSharedContext()->m_textureManager;
	
	
	const GUI_Style& currentStyle = m_styles[m_state];
	m_visual.m_backgroundSolid.setSize(currentStyle.m_size);
	m_visual.m_backgroundSolid.setFillColor(currentStyle.m_backgroundColor);
	m_visual.m_backgroundSolid.setPosition(m_position);
	m_visual.m_backgroundSolid.getFillColor();
	
	if (currentStyle.m_backgroundImage != "") {
		if (currentStyle.m_backgroundImageFullElement) {
			float intefaceWidth = m_styles[m_state].m_size.x;
			float interfaceHeight = m_styles[m_state].m_size.y;
			float scaleX = intefaceWidth / textureMgr->GetResource(currentStyle.m_backgroundImage)->getSize().x;
			float scaleY = interfaceHeight / textureMgr->GetResource(currentStyle.m_backgroundImage)->getSize().y;
			m_visual.m_backgroundImage.setScale(scaleX, scaleY);
		}
		m_visual.m_backgroundImage.setColor(currentStyle.m_backgroundImageColor);
		m_visual.m_backgroundImage.setTexture(*textureMgr->GetResource(currentStyle.m_backgroundImage));
		m_visual.m_backgroundImage.setPosition(m_position);
		//sets the size to gui_tileset appropriate to the size of the texture made in shitty way may cause the problem in the future
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


void GUI_Element::SetWorld(World* i_world)
{
	m_world = i_world;
}

World* GUI_Element::GetWorld()
{
	return m_world;
}

void GUI_Element::SetWindow(Window* i_wind)
{
	m_wind = i_wind;
}

Window* GUI_Element::GetWindow()
{
	return m_wind;
}

void GUI_Element::SetTextureManager(TextureManager* i_textureManager)
{
	m_textureManager = i_textureManager;
}

TextureManager* GUI_Element::GetTextureManager()
{
	return m_textureManager;
}

void GUI_Element::SetZoomLevel()
{
	m_zoomLevel = 1.f;
	sf::Vector2f size = GetSize();
	m_view.setSize(size);
	m_view.setCenter(size.x * 0.5f, size.y * 0.5f);
}

float GUI_Element::GetZoomLevel()
{
	return m_zoomLevel;
}

void GUI_Element::SetLayer(const int& i_layer)
{
	m_layerIndex = i_layer;
}

int GUI_Element::GetLayer() {
	return m_layerIndex;
}