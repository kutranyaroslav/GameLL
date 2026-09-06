#pragma once
#include "S_Base.h"
#include "Window.h"
#include <random>

// Spawns, ages and draws the particles held by every C_Particles component.
//
// Everything is batched into one vertex array per emitter and drawn additively
// into the scene target, so the bloom pass picks the particles up.
class S_Particles : public S_Base
{
public:
	S_Particles(SystemManager* i_systemMgr);
	~S_Particles();

	void Update(float i_dT) override;
	void HandleEvent(const EntityId& i_entity, const EntityEvent& i_event) override;
	void LoadMaterial(const std::string& i_materialName) override;
	void LoadMaterials() override;
	// Particles subscribe to nothing, but Observer::Notify is pure virtual.
	void Notify(const Message& i_message) override {}

	// Draws into the window's scene target, in world space.
	void Render(Window* i_wind);

private:
	// A runaway emitter should cost a bounded amount, not the frame.
	static const size_t MAX_PER_EMITTER = 400;

	float Range(float i_min, float i_max);

	std::mt19937 m_rng;
	sf::VertexArray m_quads;
};
