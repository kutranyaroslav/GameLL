#include "GUI_Tileset.h"
#include "../GUI_Interface.h"
#include "../GUI_Manager.h"
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
	sf::Vector2f local = i_mousePos - GetGlobalPosition();

	// add scroll offset!
	local.x += m_owner->GetScrollHorizontal();
	local.y += m_owner->GetScrollVertical();

	int tileX = local.x / Sheet::Tile_Size;
	int tileY = local.y / Sheet::Tile_Size;
	m_clickedTilePos = { tileX, tileY };

	int tilesPerRow = m_styles[m_state].m_size.x  / Sheet::Tile_Size;

	m_selectedTileId = tileY * tilesPerRow + tileX;
	float x =
		std::round(GetGlobalPosition().x)
		+ tileX * Sheet::Tile_Size
		- std::round(m_owner->GetScrollHorizontal());

	float y =
		std::round(GetGlobalPosition().y)
		+ tileY * Sheet::Tile_Size
		- std::round(m_owner->GetScrollVertical());
	m_selectedTile.setPosition(x, y);

	SetRedraw(true);
}
void GUI_Tileset::OnRelease() {
	SetState(GUI_ElementState::Neutral);
}
void GUI_Tileset::OnHover(const sf::Vector2f& i_mousePos) {
	m_hoverTile.setFillColor(m_hoverTileDefColor);
	sf::Vector2f local = i_mousePos - GetGlobalPosition();
	local.x += m_owner->GetScrollHorizontal();
	local.y += m_owner->GetScrollVertical();
	int tileX = local.x / Sheet::Tile_Size; 
	int tileY = local.y / Sheet::Tile_Size;
	m_hoverTilePos  = { tileX, tileY };


	float x =
			std::round(GetGlobalPosition().x)
			+ tileX * Sheet::Tile_Size
			- std::round(m_owner->GetScrollHorizontal());

	float y =
			std::round(GetGlobalPosition().y)
			+ tileY * Sheet::Tile_Size
			- std::round(m_owner->GetScrollVertical());
	m_hoverTile.setPosition(x, y);
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

void GUI_Tileset::ApplyBgStyle()
{
	GUI_Element::ApplyBgStyle();
	m_styles[GUI_ElementState::Neutral].m_size = sf::Vector2f(m_visual.m_backgroundImage.getTextureRect().getSize());
	m_styles[GUI_ElementState::Clicked].m_size = sf::Vector2f(m_visual.m_backgroundImage.getTextureRect().getSize());
	m_styles[GUI_ElementState::Focused].m_size = sf::Vector2f(m_visual.m_backgroundImage.getTextureRect().getSize());
}

void GUI_Tileset::SetTileset(const std::string& name)
{
	auto* texMgr = m_owner->GetGuiManager()->GetSharedContext()->m_textureManager;
	auto* tex = texMgr->GetResource(name);

	m_visual.m_backgroundImage.setTexture(*tex);
	m_styles[GUI_ElementState::Neutral].m_size = sf::Vector2f(tex->getSize());
	m_styles[GUI_ElementState::Clicked].m_size = sf::Vector2f(tex->getSize());
	m_styles[GUI_ElementState::Focused].m_size = sf::Vector2f(tex->getSize());
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

	TileKey key{ m_selectedTileId, m_hoverTilePos.y };

	auto itr = tileset->find(key);

	if (itr == tileset->end())
	{
		return nullptr;
	}

	return itr->second;
}