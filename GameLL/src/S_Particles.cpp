#include "S_Particles.h"
#include "C_Position.h"
#include "C_Particles.h"
#include "SystemManager.h"
#include <cmath>

namespace {
	const float PI = 3.14159265f;
}

S_Particles::S_Particles(SystemManager* i_systemMgr) :
	S_Base(System::Particles, i_systemMgr), m_rng(20260906u) {
	Bitmask req;
	req.turnOnBit((unsigned int)Component::Position);
	req.turnOnBit((unsigned int)Component::Particles);
	m_requiredComponents.push_back(req);
	req.Clear();

	m_quads.setPrimitiveType(sf::Quads);
}

S_Particles::~S_Particles() {}

void S_Particles::HandleEvent(const EntityId& i_entity, const EntityEvent& i_event) {}
void S_Particles::LoadMaterial(const std::string& i_materialName) {}
void S_Particles::LoadMaterials() {}

float S_Particles::Range(float i_min, float i_max) {
	std::uniform_real_distribution<float> dist(i_min, i_max);
	return dist(m_rng);
}

void S_Particles::Update(float i_dT) {
	EntityManagerNew* entities = m_systemMgr->GetEntityManager();
	if (!entities) { return; }
	if (i_dT <= 0.f) { return; }

	for (auto& entity : m_entities) {
		C_Position* position = entities->GetComponent<C_Position>(entity, Component::Position);
		C_Particles* emitter = entities->GetComponent<C_Particles>(entity, Component::Particles);
		if (!position || !emitter) { continue; }

		std::vector<C_Particles::Particle>& pool = emitter->GetParticles();

		// Age first, then spawn, so a particle created this frame gets its full
		// lifetime rather than one frame less.
		for (size_t i = 0; i < pool.size();) {
			pool[i].life -= i_dT;
			if (pool[i].life <= 0.f) {
				// Order does not matter, so fill the hole from the back rather
				// than shifting everything down.
				pool[i] = pool.back();
				pool.pop_back();
				continue;
			}
			pool[i].position += pool[i].velocity * i_dT;
			++i;
		}

		float& carry = emitter->GetAccumulator();
		carry += emitter->GetRate() * i_dT;
		while (carry >= 1.f) {
			carry -= 1.f;
			if (pool.size() >= MAX_PER_EMITTER) { break; }

			// Centred on straight up, which is negative Y in screen space.
			const float half = emitter->GetSpread() * 0.5f;
			const float angle = (-90.f + Range(-half, half)) * PI / 180.f;
			const float speed = emitter->GetSpeed() * Range(0.6f, 1.f);

			C_Particles::Particle p;
			p.position = position->GetPosition();
			p.velocity = sf::Vector2f(std::cos(angle) * speed, std::sin(angle) * speed);
			p.maxLife = emitter->GetLifetime() * Range(0.7f, 1.f);
			p.life = p.maxLife;
			pool.push_back(p);
		}
	}
}

void S_Particles::Render(Window* i_wind) {
	if (!i_wind) { return; }
	sf::RenderTexture* scene = i_wind->GetSceneTexture();
	if (!scene || scene->getSize().x == 0) { return; }

	EntityManagerNew* entities = m_systemMgr->GetEntityManager();
	if (!entities) { return; }

	for (auto& entity : m_entities) {
		C_Particles* emitter = entities->GetComponent<C_Particles>(entity, Component::Particles);
		if (!emitter) { continue; }

		std::vector<C_Particles::Particle>& pool = emitter->GetParticles();
		if (pool.empty()) { continue; }

		const sf::Color base = emitter->GetColour();
		const float half = emitter->GetSize() * 0.5f;

		m_quads.clear();
		m_quads.resize(pool.size() * 4);
		for (size_t i = 0; i < pool.size(); ++i) {
			const sf::Vector2f& c = pool[i].position;
			// Fade out over the particle's life.
			const float t = pool[i].maxLife > 0.f ? (pool[i].life / pool[i].maxLife) : 0.f;
			const sf::Color colour(base.r, base.g, base.b,
				static_cast<sf::Uint8>(255.f * (t < 0.f ? 0.f : (t > 1.f ? 1.f : t))));

			m_quads[i * 4 + 0] = sf::Vertex(sf::Vector2f(c.x - half, c.y - half), colour);
			m_quads[i * 4 + 1] = sf::Vertex(sf::Vector2f(c.x + half, c.y - half), colour);
			m_quads[i * 4 + 2] = sf::Vertex(sf::Vector2f(c.x + half, c.y + half), colour);
			m_quads[i * 4 + 3] = sf::Vertex(sf::Vector2f(c.x - half, c.y + half), colour);
		}
		// Additive so overlapping particles build up into something the bright
		// pass will pick out.
		scene->draw(m_quads, sf::BlendAdd);
	}
}
