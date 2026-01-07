#pragma once
#include "C_Base.h"
#include "SFML/Graphics.hpp"
class C_Position: public C_Base
{
public:
	C_Position(): C_Base(Component::Position), m_elevation(0){ }
	~C_Position() {};
	void ReadIn(std::stringstream& i_stream) {
		i_stream >> m_position.x >> m_position.y >> m_elevation;
	}

	const sf::Vector2f GetPosition() { return m_position; }
	const sf::Vector2f GetOldPosition() { return m_positionOld; }
	unsigned int getElevation() { return m_elevation; }
	void SetPosition(float i_x, float i_y) {
		m_positionOld = m_position; 
		m_position = sf::Vector2f(i_x, i_y);
	}
	void SetPosition(sf::Vector2f i_pos) {
		m_positionOld = m_position;
		m_position = i_pos;
	}
	void SetElevation(unsigned int i_elev) {
		m_elevation = i_elev;
	}
	void MoveBy(float i_x, float i_y) {
		m_positionOld = m_position; 
		m_position += sf::Vector2f(i_x, i_y);
	}
	void MoveBy(sf::Vector2f i_pos) {
		m_positionOld = m_position; 
		m_position += i_pos;
	}
private:
	sf::Vector2f m_position;
	sf::Vector2f m_positionOld;
	unsigned int m_elevation;
};

