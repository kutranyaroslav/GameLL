	#include "GUI_Tileset.h"
#include "../GUI_Interface.h"
#include "../GUI_Manager.h"
#include <algorithm>
#include <cmath>
#include <iostream>
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
	m_hitboxOutline.setFillColor(sf::Color(0, 0, 0, 0));
	m_hitboxOutline.setOutlineColor(sf::Color(255, 90, 70, 255));
	m_hitboxOutline.setOutlineThickness(-2.f);
	
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
		+ tileX * tileSize;

	float y =
		std::round(GetGlobalPosition().y)
		+ tileY * tileSize;
	m_selectedTile.setPosition(x, y);
	m_selectedTile.setSize(sf::Vector2f(tileSize, tileSize));
	if (m_hitboxEditMode) {
		m_draggingHitbox = true;
		m_hitboxDragStart = sf::Vector2f(local.x - tileX * tileSize,
			local.y - tileY * tileSize) / m_scale;
		m_hitboxDragEnd = m_hitboxDragStart;
		UpdateHitboxFromMouse(i_mousePos);
	}

	SetRedraw(true);
}
void GUI_Tileset::OnRelease() {
	if (m_draggingHitbox && m_world && m_world->GetCurrentMap()) {
		if (TileInfo* tile = GetSelectedTileInfo()) {
			std::string tilesetName = m_name;
			const std::string suffix = "_Tileset";
			if (tilesetName.size() >= suffix.size()) {
				tilesetName.erase(tilesetName.size() - suffix.size());
			}
			if (!m_world->GetCurrentMap()->SaveTilesetHitbox(tilesetName, tile)) {
				std::cerr << "Could not save hitbox for tile " << tile->m_id << ':'
					<< tile->m_row << " in tileset " << tilesetName << '\n';
			}
		}
	}
	m_draggingHitbox = false;
	SetState(GUI_ElementState::Neutral);
}
void GUI_Tileset::OnHover(const sf::Vector2f& i_mousePos) {
	m_hoverTile.setFillColor(m_hoverTileDefColor);
	const float tileSize = GetScaledTileSize();
	sf::Vector2f local = i_mousePos - GetGlobalPosition();
	int tileX = static_cast<int>(local.x / tileSize);
	int tileY = static_cast<int>(local.y / tileSize);
	m_hoverTilePos  = { tileX, tileY };


	float x =
			std::round(GetGlobalPosition().x)
			+ tileX * tileSize;

	float y =
			std::round(GetGlobalPosition().y)
			+ tileY * tileSize;
	m_hoverTile.setPosition(x, y);
	m_hoverTile.setSize(sf::Vector2f(tileSize, tileSize));
	if (m_draggingHitbox) {
		UpdateHitboxFromMouse(i_mousePos);
		// GUI_Interface dispatches OnRelease only to children still in Clicked state.
		SetState(GUI_ElementState::Clicked);
	}
	else { SetState(GUI_ElementState::Focused); }
}
void GUI_Tileset::OnLeave() {
	SetState(m_draggingHitbox ? GUI_ElementState::Clicked : GUI_ElementState::Neutral);
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
	if (TileInfo* tile = GetSelectedTileInfo()) {
		const float scale = (m_scale > 0.f ? m_scale : 1.f);
		const float tileSize = GetScaledTileSize();
		const sf::Vector2f tilePosition = GetGlobalPosition() + sf::Vector2f(
			m_clickedTilePos.x * tileSize, m_clickedTilePos.y * tileSize);
		const sf::FloatRect bounds = tile->GetCollisionBounds();
		m_hitboxOutline.setPosition(tilePosition.x + bounds.left * scale,
			tilePosition.y + bounds.top * scale);
		m_hitboxOutline.setSize(sf::Vector2f(bounds.width * scale, bounds.height * scale));
		m_hitboxOutline.setOutlineColor(m_hitboxEditMode
			? sf::Color(255, 90, 70, 255)
			: (tile->m_solid ? sf::Color(255, 90, 70, 255) : sf::Color(90, 190, 220, 255)));
		m_hitboxOutline.setOutlineThickness(-std::max(1.f, 2.f * scale));
		i_target->draw(m_hitboxOutline);
	}
	
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
	if (!m_world || !m_world->GetCurrentMap()) { return nullptr; }
	std::string tilesetName = m_name;

	const std::string suffix = "_Tileset";
	if (tilesetName.size() >= suffix.size())
	{
		tilesetName.erase(tilesetName.size() - suffix.size());
	}

	TileSet* tileset = m_world->GetCurrentMap()->GetTileset(tilesetName);
	if (!tileset) { return nullptr; }

	TileKey key{ m_clickedTilePos.x, m_clickedTilePos.y };

	auto itr = tileset->find(key);

	if (itr == tileset->end())
	{
		return nullptr;
	}
	itr->second->m_tilesetName = tilesetName;

	return itr->second;
}

void GUI_Tileset::SetHitboxEditMode(bool i_enabled)
{
	m_hitboxEditMode = i_enabled;
	if (!i_enabled) { m_draggingHitbox = false; }
	SetRedraw(true);
}

bool GUI_Tileset::GetHitboxEditMode() const
{
	return m_hitboxEditMode;
}

bool GUI_Tileset::ToggleSelectedTileSolidity()
{
	TileInfo* tile = GetSelectedTileInfo();
	if (!tile || !m_world || !m_world->GetCurrentMap()) { return false; }
	std::string tilesetName = m_name;
	const std::string suffix = "_Tileset";
	if (tilesetName.size() >= suffix.size()) {
		tilesetName.erase(tilesetName.size() - suffix.size());
	}
	const bool previous = tile->m_solid;
	tile->m_solid = !previous;
	if (!m_world->GetCurrentMap()->SaveTilesetHitbox(tilesetName, tile)) {
		tile->m_solid = previous;
		std::cerr << "Could not save solidity for tile " << tile->m_id << ':' << tile->m_row
			<< " in tileset " << tilesetName << '\n';
		return false;
	}
	SetRedraw(true);
	return true;
}

void GUI_Tileset::UpdateHitboxFromMouse(const sf::Vector2f& i_mousePos)
{
	if (!m_hitboxEditMode || !m_draggingHitbox || m_clickedTilePos.x < 0 ||
		m_clickedTilePos.y < 0) { return; }
	const float scale = (m_scale > 0.f ? m_scale : 1.f);
	const float tileSize = GetScaledTileSize();
	const sf::Vector2f tilePosition = GetGlobalPosition() + sf::Vector2f(
		m_clickedTilePos.x * tileSize, m_clickedTilePos.y * tileSize);
	const sf::Vector2f local = (i_mousePos - tilePosition) / scale;
	m_hitboxDragEnd.x = std::clamp(local.x, 0.f, static_cast<float>(Sheet::Tile_Size));
	m_hitboxDragEnd.y = std::clamp(local.y, 0.f, static_cast<float>(Sheet::Tile_Size));
	const float left = std::min(m_hitboxDragStart.x, m_hitboxDragEnd.x);
	const float top = std::min(m_hitboxDragStart.y, m_hitboxDragEnd.y);
	const float right = std::max(m_hitboxDragStart.x, m_hitboxDragEnd.x);
	const float bottom = std::max(m_hitboxDragStart.y, m_hitboxDragEnd.y);
	if (TileInfo* tile = GetSelectedTileInfo()) {
		tile->m_collisionBounds = sf::FloatRect(left, top,
			std::max(1.f, right - left), std::max(1.f, bottom - top));
		tile->m_hasCustomCollisionBounds = true;
		// The collision system only tests tiles marked solid, so creating a
		// hitbox also enables collision for this tile.
		tile->m_solid = true;
		SetRedraw(true);
	}
}
