#include "GUI_Viewport.h"
#include "GUI_Interface.h"
GUI_Viewport::GUI_Viewport(const std::string& i_name, GUI_Interface* i_owner):
	GUI_Element(i_name, GUI_ElementType::Viewport, i_owner), m_hoverTilePos(-1 ,-1 )
{
}

void GUI_Viewport::ReadIn(std::stringstream& i_stream)
{
}

void GUI_Viewport::OnClick(const sf::Vector2f& i_mousePos)
{
	SetState(GUI_ElementState::Clicked);
}

void GUI_Viewport::OnRelease()
{
	SetState(GUI_ElementState::Neutral);
}

void GUI_Viewport::OnHover(const sf::Vector2f& i_mousePos)
{
	SetState(GUI_ElementState::Focused);
	sf::Vector2f local = i_mousePos - GetGlobalPosition();
	local.x += m_owner->GetScrollHorizontal(); 
	local.y += m_owner->GetScrollVertical();
	//to do tilex and y always cause of m_styles[m_state].m_size.x and m_styles[m_state].m_size.y being 0, so we need to check for that
	int tileX = local.x / Sheet::Tile_Size;
	int tileY = local.y / Sheet::Tile_Size;
	m_hoverTilePos = { tileX, tileY };
	float x =
		std::round(GetGlobalPosition().x)
		+ tileX * Sheet::Tile_Size
		- std::round(m_owner->GetScrollHorizontal());

	float y =
		std::round(GetGlobalPosition().y)
		+ tileY * Sheet::Tile_Size
		- std::round(m_owner->GetScrollVertical());
	m_hoverTile.setPosition(x, y);
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
		m_view.move(-300.f * i_dT, 0);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		m_view.move(300.f * i_dT, 0);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		m_view.move(0, -300.f * i_dT);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		m_view.move(0, 300.f * i_dT);
}

void GUI_Viewport::Draw(sf::RenderTarget* i_target)
{
	
}

void GUI_Viewport::DrawOverlay(sf::RenderTarget* i_target)
{
	for (int i = 0; i < Sheet::Num_Layers; i++) {
		if (m_world) {
			m_world->Draw(i);
		}
	}
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
	if (i_texture == "" || m_textureManager == nullptr || i_tileId== 0) { return; }
	if (!m_textureManager->RequireResource(i_texture)) { return; }
	m_hoverTile.setTexture(*m_textureManager->GetResource(i_texture));
	int tilesPerRow = m_textureManager->GetResource(i_texture)->getSize().x / Sheet::Tile_Size;
	int x = i_tileId % tilesPerRow;
	int y = i_tileId / tilesPerRow;
	m_hoverTile.setTextureRect(sf::IntRect(x * Sheet::Tile_Size, y * Sheet::Tile_Size, Sheet::Tile_Size, Sheet::Tile_Size));
}

