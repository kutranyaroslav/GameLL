#include "S_Lighting.h"
#include "C_Position.h"
#include "C_LightSource.h"
#include "SystemManager.h"
#include <cmath>

S_Lighting::S_Lighting(SystemManager* i_systemMgr) :S_Base(System::Lighting, i_systemMgr) {
	Bitmask req;
	req.turnOnBit((unsigned int)Component::Position);
	req.turnOnBit((unsigned int)Component::LightSource);
	m_requiredComponents.push_back(req);
	req.Clear();

	m_fan.setPrimitiveType(sf::TriangleFan);
	m_fan.resize(LIGHT_SEGMENTS + 2);
}

S_Lighting::~S_Lighting() {}

void S_Lighting::Update(float i_dT) {}
void S_Lighting::HandleEvent(const EntityId& i_entity, const EntityEvent& i_event) {}
void S_Lighting::LoadMaterial(const std::string& i_materialName) {}
void S_Lighting::LoadMaterials() {}

void S_Lighting::Render(Window* i_wind) {
	if (!i_wind) { return; }
	sf::RenderTexture* lightmap = i_wind->GetLightmapTexture();
	sf::RenderTexture* scene = i_wind->GetSceneTexture();
	if (!lightmap || !scene) { return; }
	if (lightmap->getSize().x == 0 || lightmap->getSize().y == 0) { return; }

	// Ambient first: everything the lights miss keeps this much of its colour.
	lightmap->clear(m_ambient);
	// Lights are placed in world coordinates, so the map has to be looking at
	// the same part of the world the scene is.
	lightmap->setView(scene->getView());

	EntityManagerNew* entities = m_systemMgr->GetEntityManager();
	if (!entities) { lightmap->display(); return; }

	for (auto& entity : m_entities) {
		C_Position* position = entities->GetComponent<C_Position>(entity, Component::Position);
		C_LightSource* light = entities->GetComponent<C_LightSource>(entity, Component::LightSource);
		if (!position || !light) { continue; }

		const sf::Vector2f centre = position->GetPosition() + light->GetOffset();
		const float radius = light->GetRadius();
		if (radius <= 0.f) { continue; }

		const sf::Color base = light->GetColour();
		const float intensity = light->GetIntensity() < 0.f ? 0.f : light->GetIntensity();
		const float peak = intensity > 1.f ? 1.f : intensity;
		sf::Color hot(
			static_cast<sf::Uint8>(base.r * peak),
			static_cast<sf::Uint8>(base.g * peak),
			static_cast<sf::Uint8>(base.b * peak));
		sf::Color rim(base.r, base.g, base.b, 0);

		m_fan[0] = sf::Vertex(centre, hot);
		for (unsigned int i = 0; i <= LIGHT_SEGMENTS; ++i) {
			const float angle = (2.f * 3.14159265f * static_cast<float>(i))
				/ static_cast<float>(LIGHT_SEGMENTS);
			m_fan[i + 1] = sf::Vertex(
				sf::Vector2f(centre.x + std::cos(angle) * radius,
					centre.y + std::sin(angle) * radius),
				rim);
		}
		// Additive so overlapping lights build up instead of replacing.
		lightmap->draw(m_fan, sf::BlendAdd);
	}

	lightmap->display();
}
