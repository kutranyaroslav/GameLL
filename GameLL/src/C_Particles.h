#pragma once
#include "C_Base.h"
#include "SFML/Graphics.hpp"
#include <vector>

// A continuous emitter attached to the entity's position.
//
// .entity line: Component 9 <rate> <lifetime> <speed> <spreadDegrees> <size> <r> <g> <b>
// for example:  Component 9 24 1.4 40 70 3 255 170 90
//
// The live particles are kept here rather than in the system so they are
// destroyed with the entity and no pool can outlive its owner.
class C_Particles : public C_Base
{
public:
	struct Particle {
		sf::Vector2f position;
		sf::Vector2f velocity;
		float life = 0.f;
		float maxLife = 1.f;
	};

	C_Particles() :C_Base(Component::Particles) {}
	~C_Particles() {}

	void ReadIn(std::stringstream& i_stream) {
		int r = 255;
		int g = 255;
		int b = 255;
		i_stream >> m_rate >> m_lifetime >> m_speed >> m_spread >> m_size >> r >> g >> b;
		m_colour = sf::Color(static_cast<sf::Uint8>(r), static_cast<sf::Uint8>(g),
			static_cast<sf::Uint8>(b));
	}

	float GetRate() const { return m_rate; }
	float GetLifetime() const { return m_lifetime; }
	float GetSpeed() const { return m_speed; }
	float GetSpread() const { return m_spread; }
	float GetSize() const { return m_size; }
	const sf::Color& GetColour() const { return m_colour; }

	std::vector<Particle>& GetParticles() { return m_particles; }
	float& GetAccumulator() { return m_accumulator; }

private:
	float m_rate = 20.f;        // particles per second
	float m_lifetime = 1.2f;    // seconds
	float m_speed = 35.f;       // world units per second
	float m_spread = 60.f;      // degrees of cone, centred upwards
	float m_size = 3.f;         // world units per side
	sf::Color m_colour = sf::Color::White;

	std::vector<Particle> m_particles;
	// Fractional particles carried between frames so a low rate still emits
	// evenly instead of rounding down to nothing every frame.
	float m_accumulator = 0.f;
};
