#include "GUI_Viewport.h"
#include "GUI_Interface.h"
#include <cmath>
GUI_Viewport::GUI_Viewport(const std::string& i_name, GUI_Interface* i_owner):
	GUI_Element(i_name, GUI_ElementType::Viewport, i_owner), m_hoverTilePos(-1 ,-1 ),
	m_cameraSpeed(300.f)
{
	m_view.reset(sf::FloatRect(0.f, 0.f, 1.f, 1.f));
}

GUI_Viewport::~GUI_Viewport()
{
	// The element can be destroyed before SetContext() was ever called.
	if (m_context && m_context->m_eventManager) {
		m_context->m_eventManager->RemoveCallback(StateType::Developement, "Key_S");
	}
}

void GUI_Viewport::ReadIn(std::stringstream& i_stream)
{
	i_stream >> m_cameraSpeed;
}

sf::Vector2f GUI_Viewport::GetWorldPerPixel()
{
	// The view keeps the world size it was given at creation while the element
	// scales with the window, which is what makes map tiles scale with the screen.
	// Every screen <-> world conversion has to go through this ratio.
	const sf::Vector2f size = GetSize();
	const sf::Vector2f viewSize = m_view.getSize();
	return sf::Vector2f(
		(size.x > 0.f && viewSize.x > 0.f) ? viewSize.x / size.x : 1.f,
		(size.y > 0.f && viewSize.y > 0.f) ? viewSize.y / size.y : 1.f);
}

void GUI_Viewport::OnClick(const sf::Vector2f& i_mousePos)
{
	SetState(GUI_ElementState::Clicked);
	const sf::Vector2f wpp = GetWorldPerPixel();
	const sf::Vector2f viewTopLeft = m_view.getCenter() - m_view.getSize() * 0.5f;
	const sf::Vector2f local = i_mousePos - GetGlobalPosition();
	const sf::Vector2f worldPos(viewTopLeft.x + local.x * wpp.x,
		viewTopLeft.y + local.y * wpp.y);

	m_clickedTilePos.x = static_cast<int>(std::floor(worldPos.x / Sheet::Tile_Size));
	m_clickedTilePos.y = static_cast<int>(std::floor(worldPos.y / Sheet::Tile_Size));
	ChangeMap();
}

void GUI_Viewport::OnRelease()
{
	SetState(GUI_ElementState::Neutral);	
}

void GUI_Viewport::OnHover(const sf::Vector2f& i_mousePos)
{
	SetState(GUI_ElementState::Focused);
	const sf::Vector2f wpp = GetWorldPerPixel();
	const sf::Vector2f viewTopLeft = m_view.getCenter() - m_view.getSize() * 0.5f;
	const sf::Vector2f local = i_mousePos - GetGlobalPosition();
	const sf::Vector2f worldPos(viewTopLeft.x + local.x * wpp.x,
		viewTopLeft.y + local.y * wpp.y);
	const int tileX = static_cast<int>(std::floor(worldPos.x / Sheet::Tile_Size));
	const int tileY = static_cast<int>(std::floor(worldPos.y / Sheet::Tile_Size));

	m_hoverTilePos = { tileX, tileY };
	// The brush is drawn straight to the window, not through m_view, so world
	// offsets are divided back into pixels and the sprite is scaled so that one
	// texture tile covers exactly one on-screen tile.
	const float drawX = std::round(GetGlobalPosition().x) +
		(tileX * static_cast<float>(Sheet::Tile_Size) - viewTopLeft.x) / wpp.x;

	const float drawY = std::round(GetGlobalPosition().y) +
		(tileY * static_cast<float>(Sheet::Tile_Size) - viewTopLeft.y) / wpp.y;
	m_hoverTile.setPosition(drawX, drawY);
	m_hoverTile.setScale(1.f / wpp.x, 1.f / wpp.y);
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

void GUI_Viewport::UpdateViewportRect()
{
	Window* wind = GetWindow();
	if (!wind) { return; }
	const sf::Vector2u windowSize = wind->GetWindowSize();
	if (windowSize.x == 0 || windowSize.y == 0) { return; }
	const sf::Vector2f globalPos = GetGlobalPosition();
	const sf::Vector2f size = GetSize();
	m_view.setViewport(sf::FloatRect(
		globalPos.x / windowSize.x, globalPos.y / windowSize.y,
		size.x / windowSize.x, size.y / windowSize.y));
}

void GUI_Viewport::OnResize(const sf::Vector2f& i_scale)
{
	GUI_Element::OnResize(i_scale);
	// Update the rect now rather than waiting for the next DrawOverlay: for one
	// frame after a resize, hit-testing would otherwise use the new element
	// geometry while the renderer still had the previous rect, so a click in
	// that frame landed on the wrong tile.
	UpdateViewportRect();
}

void GUI_Viewport::DrawOverlay(sf::RenderTarget* i_target)
{
	UpdateCamera(static_cast<sf::RenderWindow*>(i_target));
	Window* wind = GetWindow();
	if (!wind) { return; }
	const sf::Vector2u windowSize = wind->GetWindowSize();
	if (windowSize.x == 0 || windowSize.y == 0) { return; }
	UpdateViewportRect();
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

void GUI_Viewport::CallbackSetup()
{
	if (m_context) {
		if (m_context->m_eventManager)
		{
			m_context->m_eventManager->AddCallback(StateType::Developement, "Key_S", &GUI_Viewport::React, this);
		}
	}
	// Called once the window and geometry are set, so the viewport rect is right
	// from the first frame instead of only after the first DrawOverlay.
	UpdateViewportRect();
}

void GUI_Viewport::UpdateCamera(sf::RenderWindow* window)
{

	sf::FloatRect viewSpace(
		m_view.getCenter() - m_view.getSize() / 2.f,
		m_view.getSize());

	if (!m_world || !m_world->GetCurrentMap()) { return; }
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
void GUI_Viewport::ChangeMap() {
	if (!m_world) { return; }
	Map* m = m_world->GetCurrentMap();
	if (!m) { return; }
	if (m_clickedTileInfo == nullptr) {
		m->RemoveTile(m_clickedTilePos.x, m_clickedTilePos.y, m_layerIndex);
		return;
	}
	m->PlaceTile(m_clickedTilePos.x, m_clickedTilePos.y, m_layerIndex, m_clickedTileInfo);
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

void GUI_Viewport::React(EventDetails* i_details)
{

	//logic for saving map 
	if (!m_world) { return; }
	if (!m_world->GetCurrentMap()) { return; }
	m_world->GetCurrentMap()->SaveTiles();

}

