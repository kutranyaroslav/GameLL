#include "GUI_Viewport.h"
#include "GUI_Interface.h"
GUI_Viewport::GUI_Viewport(const std::string& i_name, GUI_Interface* i_owner):
	GUI_Element(i_name, GUI_ElementType::Viewport, i_owner), m_hoverTilePos(-1 ,-1 ),
	m_cameraSpeed(300.f)
{
	m_view.reset(sf::FloatRect(0.f, 0.f, 1.f, 1.f));
}

void GUI_Viewport::ReadIn(std::stringstream& i_stream)
{
	i_stream >> m_cameraSpeed;
}

void GUI_Viewport::OnClick(const sf::Vector2f& i_mousePos)
{
	SetState(GUI_ElementState::Clicked);
	sf::Vector2f local = i_mousePos - GetGlobalPosition();
	sf::Vector2f worldPos;
	worldPos.x = local.x + (m_view.getCenter().x - m_view.getSize().x * 0.5f);
	worldPos.y = local.y + (m_view.getCenter().y - m_view.getSize().y * 0.5f);

	m_clickedTilePos.x = static_cast<int>(worldPos.x) / Sheet::Tile_Size;
	m_clickedTilePos.y = static_cast<int>(worldPos.y) / Sheet::Tile_Size;
	ChangeMap();
}

void GUI_Viewport::OnRelease()
{
	SetState(GUI_ElementState::Neutral);
}

void GUI_Viewport::OnHover(const sf::Vector2f& i_mousePos)
{
	SetState(GUI_ElementState::Focused);
	sf::Vector2f local = i_mousePos - GetGlobalPosition();
	sf::Vector2f worldPos;
	worldPos.x = local.x + (m_view.getCenter().x - m_view.getSize().x * 0.5f);
	worldPos.y = local.y + (m_view.getCenter().y - m_view.getSize().y * 0.5f);
	int tileX = static_cast<int>(worldPos.x / Sheet::Tile_Size);
	int tileY = static_cast<int>(worldPos.y / Sheet::Tile_Size);

	m_hoverTilePos = { tileX, tileY };
	float drawX = std::round(GetGlobalPosition().x) +
		(tileX * Sheet::Tile_Size -
			(m_view.getCenter().x - m_view.getSize().x * 0.5f));

	float drawY = std::round(GetGlobalPosition().y) +
		(tileY * Sheet::Tile_Size -
			(m_view.getCenter().y - m_view.getSize().y * 0.5f));
	m_hoverTile.setPosition(drawX, drawY);
}

void GUI_Viewport::OnLeave()
{
	SetState(GUI_ElementState::Neutral);
	m_hoverTilePos = { -1,-1 };
	

}

void GUI_Viewport::Update(float i_dT)
{	
	// made not through bindings because in gui elements we have no access to gui_manager to get sharedcontext
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		m_view.move(-m_cameraSpeed * i_dT, 0);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		m_view.move(m_cameraSpeed * i_dT, 0);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		m_view.move(0, -m_cameraSpeed * i_dT);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		m_view.move(0, m_cameraSpeed * i_dT);

}

void GUI_Viewport::Draw(sf::RenderTarget* i_target)
{
	
}

void GUI_Viewport::DrawOverlay(sf::RenderTarget* i_target)
{
	UpdateCamera(static_cast<sf::RenderWindow*>(i_target));
	sf::Vector2u windowSize = GetWindow()->GetWindowSize();
	sf::Vector2f globalPos = GetGlobalPosition();
	sf::Vector2f size = GetSize();
	m_view.setViewport(sf::FloatRect(
		globalPos.x / windowSize.x, globalPos.y / windowSize.y,
		size.x / windowSize.x, size.y / windowSize.y));
	sf::View oldView = i_target->getView();
	i_target->setView(m_view);
	for (int i = 0; i < Sheet::Num_Layers; i++) {
		if (m_world) {
			m_world->Draw(*i_target, m_view,i);
		}
	}
	i_target->setView(oldView);
	if (m_hoverTilePos.x != -1 && m_hoverTilePos.y != -1) {
		i_target->draw(m_hoverTile);
	}
}

void GUI_Viewport::ApplyBgStyle()
{
	GUI_Element::ApplyBgStyle();
	m_styles[GUI_ElementState::Focused].m_size = m_styles[GUI_ElementState::Neutral].m_size;
	m_styles[GUI_ElementState::Clicked].m_size = m_styles[GUI_ElementState::Neutral].m_size;



}

void GUI_Viewport::UpdateCamera(sf::RenderWindow* window)
{

	sf::FloatRect viewSpace(
		m_view.getCenter() - m_view.getSize() / 2.f,
		m_view.getSize());

	sf::Vector2u mapSize = m_world->GetCurrentMap()->GetMapSize();

	float mapWidth = mapSize.x * Sheet::Tile_Size;
	float mapHeight = mapSize.y * Sheet::Tile_Size;

	if (viewSpace.left < 0.f)
		m_view.setCenter(m_view.getSize().x * 0.5f, m_view.getCenter().y);
	else if (viewSpace.left + viewSpace.width > mapWidth)
		m_view.setCenter(mapWidth - m_view.getSize().x * 0.5f,
			m_view.getCenter().y);

	if (viewSpace.top < 0.f)
		m_view.setCenter(m_view.getCenter().x,
			m_view.getSize().y * 0.5f);
	else if (viewSpace.top + viewSpace.height > mapHeight)
		m_view.setCenter(m_view.getCenter().x,
			mapHeight - m_view.getSize().y * 0.5f);

}
sf::View& GUI_Viewport::GetView()
{
	return m_view;
}

void GUI_Viewport::SetViewSize(const sf::Vector2f& i_size)
{
	m_view.setSize(i_size);
}

void GUI_Viewport::SetViewCenter(const sf::Vector2f& i_pos) {
	m_view.setCenter(i_pos);
}

void GUI_Viewport::SetBrush(std::string& i_texture, int i_tileId)
{
	if (i_tileId < 0) { return; }
	if (i_texture == "" || m_textureManager == nullptr) { return; }
	if (!m_textureManager->RequireResource(i_texture)) { return; }
	m_hoverTile.setTexture(*m_textureManager->GetResource(i_texture));
	int tilesPerRow = m_textureManager->GetResource(i_texture)->getSize().x / Sheet::Tile_Size;
	int x = i_tileId % tilesPerRow;
	int y = i_tileId / tilesPerRow;
	m_hoverTile.setTextureRect(sf::IntRect(x * Sheet::Tile_Size, y * Sheet::Tile_Size, Sheet::Tile_Size, Sheet::Tile_Size));
}

void GUI_Viewport::SetCameraSpeed(float i_speed)
{
	m_cameraSpeed = i_speed;
}

void GUI_Viewport::SetHoverTilesPos(const sf::Vector2i& i_pos)
{
	m_hoverTilePos = i_pos;
}
void GUI_Viewport::SetClickedTileInfo(TileInfo* i_info)
{
	m_clickedTileInfo = i_info;
}
void GUI_Viewport::ChangeMap()
{
	if (m_world)
	{
		Map* m = m_world->GetCurrentMap();
		if (m) {
			unsigned int key = m->ConvertCordinates(m_clickedTilePos.x, m_clickedTilePos.y, m_layerIndex);
			TileMap* tileMap = m->GetTileMap();
			if (tileMap) {
				auto itr = tileMap->find(key);
				if (itr == tileMap->end()) { 
					//there is no tile yet
					if (m_clickedTileInfo == nullptr) { return; }
					Tile* newTile = new Tile();
					newTile->m_properties = m_clickedTileInfo;
					tileMap->emplace(key, newTile);
					
				}
				else
				{
					//there is a tile 
					Tile* oldTile = itr->second;

				}

			}
		}
	}
}
void GUI_Viewport::ClearBrush()
{
	m_hoverTilePos = { -1, -1 };
	m_hoverTile.setTextureRect(sf::IntRect());
}

float& GUI_Viewport::GetCameraSpeed()
{
	return m_cameraSpeed;
}

