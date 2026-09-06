#pragma once
#include "C_Base.h"
#include "SFML/Graphics.hpp"

// A round light centred on the entity's position.
//
// .entity line: Component 8 <radius> <r> <g> <b> <intensity>
// for example:  Component 8 220 255 220 160 1.2
class C_LightSource : public C_Base
{
public:
	C_LightSource() :C_Base(Component::LightSource) {}
	~C_LightSource() {}

	void ReadIn(std::stringstream& i_stream) {
		int r = 255;
		int g = 255;
		int b = 255;
		i_stream >> m_radius >> r >> g >> b >> m_intensity;
		m_colour = sf::Color(static_cast<sf::Uint8>(r), static_cast<sf::Uint8>(g),
			static_cast<sf::Uint8>(b));
	}

	float GetRadius() const { return m_radius; }
	float GetIntensity() const { return m_intensity; }
	const sf::Color& GetColour() const { return m_colour; }
	const sf::Vector2f& GetOffset() const { return m_offset; }

	void SetRadius(float i_r) { m_radius = i_r; }
	void SetIntensity(float i_i) { m_intensity = i_i; }
	void SetColour(const sf::Color& i_c) { m_colour = i_c; }

private:
	float m_radius = 160.f;
	float m_intensity = 1.f;
	sf::Color m_colour = sf::Color::White;
	// Lights sit on the entity origin; sprites are drawn centred on it too.
	sf::Vector2f m_offset = sf::Vector2f(0.f, 0.f);
};
