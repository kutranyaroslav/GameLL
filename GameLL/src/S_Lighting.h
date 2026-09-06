#pragma once
#include "S_Base.h"
#include "Window.h"

// Builds the light map that the composite pass multiplies the scene by.
//
// The map starts at the ambient colour, so anywhere no light reaches keeps that
// fraction of its original brightness, and every light adds on top of it.
class S_Lighting : public S_Base
{
public:
	S_Lighting(SystemManager* i_systemMgr);
	~S_Lighting();

	void Update(float i_dT) override;
	void HandleEvent(const EntityId& i_entity, const EntityEvent& i_event) override;
	void LoadMaterial(const std::string& i_materialName) override;
	void LoadMaterials() override;
	// Lighting subscribes to nothing, but Observer::Notify is pure virtual.
	void Notify(const Message& i_message) override {}

	// Draws every light into the window's light map, in world space.
	void Render(Window* i_wind);

	void SetAmbient(const sf::Color& i_ambient) { m_ambient = i_ambient; }
	const sf::Color& GetAmbient() const { return m_ambient; }

private:
	// How round each light looks. A fan is cheaper than a texture and needs no
	// asset, and at this radius the facets are not visible.
	static const unsigned int LIGHT_SEGMENTS = 24;

	sf::Color m_ambient = sf::Color(90, 90, 105);
	sf::VertexArray m_fan;
};
