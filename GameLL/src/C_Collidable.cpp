#include "C_Collidable.h"
C_Collidable::C_Collidable() :C_Base(Component::Collidable),
m_origin(Origin::Mid_Bottom), m_collidingOnX(false),m_collidingOnY(false) {

}
void C_Collidable::ReadIn(std::stringstream& i_stream){
	unsigned int origin = 0; 
	i_stream >> m_AABB.width >> m_AABB.height >> m_offset.x >> m_offset.y >> origin;
	m_origin = (Origin)origin;
}

void C_Collidable::CollideOnX() { m_collidingOnX = true; }
void C_Collidable::CollideOnY() { m_collidingOnY = true;}
void C_Collidable::ResetCollisionFlags() { m_collidingOnX = false; m_collidingOnY = false; }
void C_Collidable::SetSize(const sf::Vector2f& i_vec) {
	m_AABB.width = i_vec.x;
	m_AABB.height = i_vec.y;
}

void C_Collidable::SetPosition(const sf::Vector2f& i_vec) {
	switch (m_origin)
	{
	case Origin::Top_Left:
		m_AABB.left = i_vec.x + m_offset.x;
		m_AABB.top = i_vec.y + m_offset.y;
		break;
	case Origin::Abs_Center:
		m_AABB.left = i_vec.x - (m_AABB.width / 2) + m_offset.x;
		m_AABB.top = i_vec.y - (m_AABB.height / 2) + m_offset.y;
		break;
	case Origin::Mid_Bottom:
		m_AABB.left = i_vec.x - (m_AABB.width / 2) + m_offset.x;
		m_AABB.top = i_vec.y + m_AABB.height + m_offset.y;
		break;
	default:
		break;
	}
}
sf::FloatRect& C_Collidable::GetCollidable() { return m_AABB; }