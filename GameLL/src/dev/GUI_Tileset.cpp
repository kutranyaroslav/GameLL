#include "GUI_Tileset.h"
#include "../GUI_Interface.h"
#include "../GUI_Manager.h"
#include <cmath>
GUI_Tileset::GUI_Tileset(const std::string& i_name, GUI_Interface* i_owner) :
	GUI_Element(i_name, GUI_ElementType::Tileset, i_owner), m_tilesetSize(0.f, 0.f), m_selectedTileId(-1)
	,m_hoverTileDefColor(0, 0, 255, 125),m_clickedTileDefColor(0,255,0,130), m_hoverTilePos(-1, -1), m_clickedTilePos(-1, -1)
{
	if(i_name != "") {
		SetTileset(i_name);
	}
	m_hoverTile.setFillColor(sf::Color(0,0,0,0));
	m_hoverTile.setSize(sf::Vector2f(Sheet::Tile_Size, Sheet::Tile_Size));
	m_selectedTile.setFillColor(sf::Color(0, 0, 0, 0));
	m_selectedTile.setSize(sf::Vector2f(Sheet::Tile_Size, Sheet::Tile_Size));
	
}

void GUI_Tileset::ReadIn(std::stringstream& i_stream){
	int rh, gh, bh, ah, rs,gs,bs,as;
	i_stream >> rh >> gh >> bh >> ah>>
		rs>>gs>>bs>>as;
	
	m_hoverTile.setFillColor(sf::Color(rh, gh, bh, ah));
	m_selectedTile.setFillColor(sf::Color(rs, gs, bs, as));
}

void GUI_Tileset::OnClick(const sf::Vector2f& i_mousePos) {
	SetState(GUI_ElementState::Clicked);
	m_selectedTile.setFillColor(m_clickedTileDefColor);
	const float tileSize = GetScaledTileSize();
	sf::Vector2f local = i_mousePos - GetGlobalPosition();

	// add scroll offset!
	local.x += m_owner->GetScrollHorizontal();
	local.y += m_owner->GetScrollVertical();

	// local is in scaled screen pixels and tileSize is the scaled tile, so the scale
	// cancels and the result is a texture tile index - which is what it has to be,
	// because m_clickedTilePos is used as a TileKey into the tileset.
	int tileX = static_cast<int>(local.x / tileSize);
	int tileY = static_cast<int>(local.y / tileSize);
	m_clickedTilePos = { tileX, tileY };

	int tilesPerRow = static_cast<int>(m_styles[m_state].m_size.x / tileSize);
	if (tilesPerRow <= 0) { tilesPerRow = 1; }

	m_selectedTileId = tileY * tilesPerRow + tileX;
	float x =
		std::round(GetGlobalPosition().x)
		+ tileX * tileSize
		- std::round(m_owner->GetScrollHorizontal());

	float y =
		std::round(GetGlobalPosition().y)
		+ tileY * tileSize
		- std::round(m_owner->GetScrollVertical());
	m_selectedTile.setPosition(x, y);
	m_selectedTile.setSize(sf::Vector2f(tileSize, tileSize));

	SetRedraw(true);
}
void GUI_Tileset::OnRelease() {
	SetState(GUI_ElementState::Neutral);
}
void GUI_Tileset::OnHover(const sf::Vector2f& i_mousePos) {
	m_hoverTile.setFillColor(m_hoverTileDefColor);
	const float tileSize = GetScaledTileSize();
	sf::Vector2f local = i_mousePos - GetGlobalPosition();
	local.x += m_owner->GetScrollHorizontal();
	local.y += m_owner->GetScrollVertical();
	int tileX = static_cast<int>(local.x / tileSize);
	int tileY = static_cast<int>(local.y / tileSize);
	m_hoverTilePos  = { tileX, tileY };


	float x =
			std::round(GetGlobalPosition().x)
			+ tileX * tileSize
			- std::round(m_owner->GetScrollHorizontal());

	float y =
			std::round(GetGlobalPosition().y)
			+ tileY * tileSize
			- std::round(m_owner->GetScrollVertical());
	m_hoverTile.setPosition(x, y);
	m_hoverTile.setSize(sf::Vector2f(tileSize, tileSize));
	SetState(GUI_ElementState::Focused);
}
void GUI_Tileset::OnLeave() {
	SetState(GUI_ElementState::Neutral);
	m_hoverTile.setFillColor(sf::Color(0, 0, 0,0));
	m_hoverTilePos = { -1,-1 };
}

void GUI_Tileset::Update(float i_dT)
{
}

void GUI_Tileset::Draw(sf::RenderTarget* i_target)
{
	i_target->draw(m_visual.m_backgroundSolid);
	i_target->draw(m_visual.m_backgroundImage);
}

void GUI_Tileset::DrawOverlay(sf::RenderTarget* i_target)
{
	i_target->draw(m_selectedTile);
	i_target->draw(m_hoverTile);
	
}

float GUI_Tileset::GetScaledTileSize()
{
	// Sheet::Tile_Size is the tile in *texture* space and never changes; m_scale is
	// how far the GUI has been scaled for the current window size.
	const float scale = (m_scale > 0.f ? m_scale : 1.f);
	return static_cast<float>(Sheet::Tile_Size) * scale;
}

void GUI_Tileset::ApplyBgStyle()
{
	GUI_Element::ApplyBgStyle();
	// The sprite is scaled along with the element box, so the drawn tileset and
	// its hit-box stay aligned.
	const float scale = (m_scale > 0.f ? m_scale : 1.f);
	m_visual.m_backgroundImage.setScale(scale, scale);
	const sf::Vector2f scaledSize =
		sf::Vector2f(m_visual.m_backgroundImage.getTextureRect().getSize()) * scale;
	m_styles[GUI_ElementState::Neutral].m_size = scaledSize;
	m_styles[GUI_ElementState::Clicked].m_size = scaledSize;
	m_styles[GUI_ElementState::Focused].m_size = scaledSize;
	// Highlights are fixed-size rectangles, so they need resizing with the tiles.
	const float tileSize = GetScaledTileSize();
	m_hoverTile.setSize(sf::Vector2f(tileSize, tileSize));
	m_selectedTile.setSize(sf::Vector2f(tileSize, tileSize));
}

void GUI_Tileset::CallbackSetup()
{
}

void GUI_Tileset::SetTileset(const std::string& name)
{
	auto* texMgr = m_owner->GetGuiManager()->GetSharedContext()->m_textureManager;
	auto* tex = texMgr->GetResource(name);

	m_visual.m_backgroundImage.setTexture(*tex);
	// Same scale rule as ApplyBgStyle, so this path cannot disagree with it.
	const float scale = (m_scale > 0.f ? m_scale : 1.f);
	const sf::Vector2f scaledSize = sf::Vector2f(tex->getSize()) * scale;
	m_visual.m_backgroundImage.setScale(scale, scale);
	m_styles[GUI_ElementState::Neutral].m_size = scaledSize;
	m_styles[GUI_ElementState::Clicked].m_size = scaledSize;
	m_styles[GUI_ElementState::Focused].m_size = scaledSize;
}


std::string& GUI_Tileset::GetTilesetTexture()
{
	return m_styles[m_state].m_backgroundImage;
}

int GUI_Tileset::GetSelectedTileId() const
{
	return m_selectedTileId;
}

TileInfo* GUI_Tileset::GetSelectedTileInfo()
{
	if (m_selectedTileId == -1) { return nullptr; }
	if (!m_world) { return nullptr; }
	std::string tilesetName = m_name;

	const std::string suffix = "_Tileset";
	if (tilesetName.size() >= suffix.size())
	{
		tilesetName.erase(tilesetName.size() - suffix.size());
	}

	TileSet* tileset = m_world->GetCurrentMap()->GetTileset(tilesetName);

	TileKey key{ m_clickedTilePos.x, m_clickedTilePos.y };

	auto itr = tileset->find(key);

	if (itr == tileset->end())
	{
		return nullptr;
	}
	itr->second->m_tilesetName = tilesetName;

	return itr->second;
}