#pragma once
#include "S_Base.h"
#include "C_Interactable.h"
#include <functional>

//message m_receiver = target entity, m_sender = actor entity
class S_Interaction : public S_Base
{
public:
	S_Interaction(SystemManager* i_systemMgr);
	void Update(float i_dT) override {}
	void HandleEvent(const EntityId& i_entity, const EntityEvent& i_event) override {}
	void Notify(const Message& i_message) override;
	void LoadMaterial(const std::string& i_materialName) override;
	void LoadMaterials() override;
	int SpawnForTile(Materials::MaterialType i_material, const sf::Vector2f& i_worldPos, unsigned int i_elevation);
private:
	using Handler = std::function<void(EntityId i_target, EntityId i_actor, C_Interactable*)>;
	std::unordered_map<InteractionType, Handler> m_handlers;
	//first is the materials name and second is an .entity file
	std::unordered_map<std::string, std::string> m_materialToTemplate;

	void HandleOpenDoor(EntityId i_target, EntityId i_actor, C_Interactable* i_data);
	void HandleHeal(EntityId i_target, EntityId i_actor, C_Interactable* i_data);
};