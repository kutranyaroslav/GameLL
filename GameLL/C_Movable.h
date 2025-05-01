#pragma once
#include "C_Base.h"
#include "SFML/Graphics.hpp"
#include "Directions.h"
class C_Movable: public C_Base
{
public:
	C_Movable();
	void ReadIn(std::stringstream& i_stream) override;
	void AddVelocity(const sf::Vector2f& i_vec);
	void ApplyFriction(const sf::Vector2f& i_vec);
	void Accelerate(float i_x, float i_y);
	void Accelerate(const sf::Vector2f& i_vec);
	void Move(const Direction& dir);
	sf::Vector2f GetVelocity();
	sf::Vector2f GetSpeed();
	sf::Vector2f GetAcceleration();
	float GetMaxVelocity();
	void SetAcceleration(const sf::Vector2f& i_vec);
	void SetVelocity(const sf::Vector2f& i_vec);
	void SetDirection(const Direction& i_dir);
private:
	sf::Vector2f m_velocity;
	float m_velocityMax;
	sf::Vector2f m_speed;
	sf::Vector2f m_acceleration;
	Direction m_direction;
};
	
