#pragma once
#include "C_Base.h"
#include "SFML/Graphics.hpp"

enum class Origin{Top_Left,Abs_Center, Mid_Bottom };
class C_Collidable:public C_Base
{
public:
	C_Collidable();
	void ReadIn(std::stringstream& i_stream) override;
	void CollideOnX();
	void CollideOnY();
	void ResetCollisionFlags();
	void SetSize(const sf::Vector2f& i_vec);
	void SetPosition(const sf::Vector2f& i_vec);
	sf::FloatRect& GetCollidable();
private:
	sf::FloatRect m_AABB;
	sf::Vector2f m_offset;
	Origin m_origin;
	bool m_collidingOnX;
	bool m_collidingOnY;
};

