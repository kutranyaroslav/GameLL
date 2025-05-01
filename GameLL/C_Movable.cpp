#include "C_Movable.h"

C_Movable::C_Movable() :C_Base(Component::Movable), m_velocityMax(0.f), m_direction((Direction)0) {

}
void C_Movable::ReadIn(std::stringstream& i_stream) {
	i_stream >> m_velocityMax >> m_speed.x >> m_speed.y; 
	unsigned int dir = 0; 
	i_stream >> dir;
	m_direction = Direction(dir);
}
void C_Movable::AddVelocity(const sf::Vector2f& i_vec) {
	m_velocity += i_vec;
	if (std::abs(m_velocity.x) > m_velocityMax) {
		m_velocity.x = m_velocityMax * (m_velocity.x / std::abs(m_velocity.x));
	}
	if (std::abs(m_velocity.y) > m_velocityMax) {
		m_velocity.y = m_velocityMax * (m_velocity.y / std::abs(m_velocity.y)); 
	}
}
void C_Movable::ApplyFriction(const sf::Vector2f& i_vec) {
	if (m_velocity.x != 0 && i_vec.x != 0) {
		if (std::abs(m_velocity.x) - std::abs(i_vec.x) < 0) {
			m_velocity.x = 0;
		}
		else {
			m_velocity.x += (m_velocity.x > 0 ? i_vec.x * -1 : i_vec.x);
		}
	}
	if (m_velocity.y != 0 && i_vec.y != 0) {
		if (std::abs(m_velocity.y) - std::abs(i_vec.y) < 0) {
			m_velocity.y = 0;
		}
		else {
			m_velocity.y += (m_velocity.y > 0 ? i_vec.y * -1 : i_vec.y);
		}
	}
}

void C_Movable::Accelerate(const sf::Vector2f& i_vec) {
	m_acceleration += i_vec;
}
void C_Movable::Accelerate(float i_x, float i_y) {
	m_acceleration += sf::Vector2f(i_x, i_y);
}

void C_Movable::Move(const Direction& i_dir) {
	if (i_dir == Direction::Up) {
		m_acceleration.y -= m_speed.y;
	}
	else if (i_dir == Direction::Down) {
		m_acceleration.y += m_speed.y;
	}
	else if (i_dir == Direction::Right) {
		m_acceleration.x -= m_speed.x;
	}
	else if (i_dir == Direction::Left) {
		m_acceleration.x += m_speed.x;
	}
}

sf::Vector2f C_Movable::GetVelocity() {
	return m_velocity;
}
sf::Vector2f C_Movable::GetSpeed() {
	return m_speed;
}
sf::Vector2f C_Movable::GetAcceleration() {
	return m_acceleration;
}
float C_Movable::GetMaxVelocity() {
	return m_velocityMax;
}

void C_Movable::SetAcceleration(const sf::Vector2f& i_vec) {
	m_acceleration = i_vec;
}
void C_Movable::SetVelocity(const sf::Vector2f& i_vec) {
	m_velocity = i_vec;
}
void C_Movable::SetDirection(const Direction& i_dir) {
	m_direction = i_dir;
}