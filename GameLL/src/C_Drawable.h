#pragma once
#include "C_Base.h"
#include "SFML/Graphics.hpp"
class C_Drawable: public C_Base
{
public:
	C_Drawable(const Component& i_type): C_Base(i_type){}
	virtual ~C_Drawable() {};
	virtual void UpdatePosition(const sf::Vector2f& i_vec) = 0;
	virtual const sf::Vector2f GetSize() = 0;
	virtual void Draw(sf::RenderWindow* i_wind) = 0;
};

