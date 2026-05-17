#include "GUI_Tileset.h"
#include "../GUI_Interface.h"
#include "../GUI_Manager.h"
GUI_Tileset::GUI_Tileset(const std::string& i_name, GUI_Interface* i_owner) :
	GUI_Element(i_name, GUI_ElementType::Tileset, i_owner), m_tilesetSize(0.f, 0.f), m_selectedTileId(-1)
{
	if(i_name != "") {
		SetTileset(i_name);
	}
}

void GUI_Tileset::ReadIn(std::stringstream& i_stream){}

void GUI_Tileset::OnClick(const sf::Vector2f& i_mousePos) {
	SetState(GUI_ElementState::Clicked);
	sf::Vector2f local = i_mousePos - GetGlobalPosition();

	// add scroll offset!
	local.x += m_owner->GetScrollHorizontal();
	local.y += m_owner->GetScrollVertical();

	int tileX = local.x / Sheet::Tile_Size;
	int tileY = local.y / Sheet::Tile_Size;

	int tilesPerRow = m_styles[m_state].m_size.x  / Sheet::Tile_Size;

	m_selectedTileId = tileY * tilesPerRow + tileX;

	SetRedraw(true);
}
void GUI_Tileset::OnRelease() {
	SetState(GUI_ElementState::Neutral);
}
void GUI_Tileset::OnHover(const sf::Vector2f& i_mousePos) {
	SetState(GUI_ElementState::Focused);
}
void GUI_Tileset::OnLeave() {
	SetState(GUI_ElementState::Neutral);
}

void GUI_Tileset::Update(float i_dT)
{
}

void GUI_Tileset::Draw(sf::RenderTarget* i_target)
{
	i_target->draw(m_visual.m_backgroundSolid);
	i_target->draw(m_visual.m_backgroundImage);
}

void GUI_Tileset::SetTileset(const std::string& name)
{
	auto* texMgr = m_owner->GetGuiManager()->GetSharedContext()->m_textureManager;
	auto* tex = texMgr->GetResource(name);

	m_visual.m_backgroundImage.setTexture(*tex);
	m_styles[m_state].m_size = sf::Vector2f(tex->getSize());
}