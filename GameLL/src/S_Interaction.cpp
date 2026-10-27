#include "S_Interaction.h"
#include "EntityManagerNew.h"
#include "StateManager.h" // for SharedContext (m_world, m_textbox)
#include "World.h"
#include "Textbox.h"
#include <algorithm>
#include <filesystem>

#include "C_Inventory.h"

S_Interaction::S_Interaction(SystemManager* i_systemMgr) :
	S_Base(System::Interaction, i_systemMgr)
{
	Bitmask req;
	req.turnOnBit((unsigned int)Component::Position);
	req.turnOnBit((unsigned int)Component::State);
	req.turnOnBit((unsigned int)Component::Interactable);
	m_requiredComponents.push_back(req);
	req.Clear();

	m_systemMgr->GetMessageHandler()->Subscribe(EntityMessage::Interaction_Action, this);
	m_systemMgr->GetMessageHandler()->Subscribe(EntityMessage::Interaction_Completed, this);
	// here we to register handle for each type of interaction, so that Notify can call the right one
	m_handlers[InteractionType::Door] = [this](EntityId t, EntityId a, C_Interactable* d) { HandleOpenDoor(t, a, d); };
	m_handlers[InteractionType::AidKit] = [this](EntityId t, EntityId a, C_Interactable* d) { HandleHeal(t, a, d); };
	m_handlers[InteractionType::PickupItem] = [this](EntityId t, EntityId a, C_Interactable* d) { HandlePickupItem(t, a, d); };
	m_handlers[InteractionType::ReadNote] = [this](EntityId t, EntityId a, C_Interactable* d) { HandleReadNote(t, a, d); };
	m_handlers[InteractionType::ToggleLever] = [this](EntityId t, EntityId a, C_Interactable* d) { HandleToggleLever(t, a, d); };
	m_handlers[InteractionType::Examine] = [this](EntityId t, EntityId a, C_Interactable* d) { HandleExamine(t, a, d); };
	m_handlers[InteractionType::SavePoint] = [this](EntityId t, EntityId a, C_Interactable* d) { HandleSavePoint(t, a, d); };
	m_handlers[InteractionType::Terminal] = [this](EntityId t, EntityId a, C_Interactable* d) { HandleTerminal(t, a, d); };
	m_handlers[InteractionType::Elevator] = [this](EntityId t, EntityId a, C_Interactable* d) { HandleElevator(t, a, d); };
	// новый тип интеракции = новая строчка тут + один маленький Handle*, без нового класс
}

S_Interaction::~S_Interaction() {
	m_systemMgr->GetSharedContext()->m_eventManager->RemoveCallback(StateType(0),"Key_E");
}

void S_Interaction::HandleEvent(const EntityId &i_entity, const EntityEvent &i_event) {

}

void S_Interaction::Notify(const Message& i_message) {
	if ((EntityMessage)i_message.m_type  == EntityMessage::Interaction_Completed) {
		C_Interactable* actorIntrcb = m_systemMgr->GetSharedContext()->
		m_entityManager->GetComponent<C_Interactable>(i_message.m_receiver, Component::Interactable);
		C_Position* actorPos = m_systemMgr->GetSharedContext()->
		m_entityManager->GetComponent<C_Position>(i_message.m_receiver, Component::Position);
		if (!actorIntrcb){return;}
		if (!actorPos){return;}
		InteractionType interaction = actorIntrcb->GetType();
		if (interaction == InteractionType::None){return;}
		//special for the door
		if (interaction == InteractionType::Door) {
			actorIntrcb->SetDoorOpened((bool)i_message.m_int);
			Map* map = m_systemMgr->GetSharedContext()->m_world->GetCurrentMap();
			TileMap* tiles = map ? map->GetTileMap() : nullptr;
			bool removedLinkedTile = false;
			if (tiles) {
				const sf::Vector2u mapSize = map->GetMapSize();
				if (mapSize.x > 0 && mapSize.y > 0) {
					for (const auto& [key, tile] : *tiles) {
						if (!tile || tile->m_linkedEntity != i_message.m_receiver) {
							continue;
						}

						// Decode the tile-map key; this matches Map::SaveTiles().
						const unsigned int x = key % mapSize.x;
						const unsigned int temp = key / mapSize.x;
						const unsigned int y = temp % mapSize.y;
						const unsigned int layer = temp / mapSize.y;
						Tile* t = map->GetTile(x,y,layer);
						t->m_properties->m_solid = i_message.m_int;
						break;
					}

				}
			}
		}
	}
}
void S_Interaction::HandleOpenDoor(EntityId i_target, EntityId i_actor, C_Interactable* i_data) {
	m_systemMgr->AddEvent(i_actor, (EventId)EntityEvent::Door_Open);
}

void S_Interaction::HandleHeal(EntityId i_target, EntityId i_actor, C_Interactable* i_data) {
	// у тебя пока нет C_Health в ECS_Types — как появится, тут будет:
	// entities->GetComponent<C_Health>(i_actor, Component::Health)->Add(i_data->GetAmount());
	m_systemMgr->AddEvent(i_actor, (EventId)EntityEvent::Healed);
}

void S_Interaction::HandlePickupItem(EntityId i_target, EntityId i_actor, C_Interactable* i_data) {
	m_systemMgr->AddEvent(i_actor, (EventId)EntityEvent::Item_Picked);
}

void S_Interaction::HandleReadNote(EntityId i_target, EntityId i_actor, C_Interactable* i_data) {
	m_systemMgr->AddEvent(i_target, (EventId)EntityEvent::Note_Read);
}

void S_Interaction::HandleToggleLever(EntityId i_target, EntityId i_actor, C_Interactable* i_data) {
	// i_target - сам рычаг, GetTargetId() - id сущности которую он контролирует (дверь, ворота и т.п.)
	int controlled = i_data->GetTargetId();
	if (controlled >= 0) {
		m_systemMgr->AddEvent((EntityId)controlled, (EventId)EntityEvent::Lever_Toggled);
	}
	m_systemMgr->AddEvent(i_target, (EventId)EntityEvent::Lever_Toggled);
}

void S_Interaction::HandleExamine(EntityId i_target, EntityId i_actor, C_Interactable* i_data) {
	m_systemMgr->AddEvent(i_target, (EventId)EntityEvent::Examined);
}

void S_Interaction::HandleSavePoint(EntityId i_target, EntityId i_actor, C_Interactable* i_data) {
	m_systemMgr->AddEvent(i_target, (EventId)EntityEvent::Game_Saved);
}

void S_Interaction::HandleTerminal(EntityId i_target, EntityId i_actor, C_Interactable* i_data) {
	m_systemMgr->AddEvent(i_target, (EventId)EntityEvent::Terminal_Used);
}

void S_Interaction::HandleElevator(EntityId i_target, EntityId i_actor, C_Interactable* i_data) {
	SharedContext* context = m_systemMgr->GetSharedContext();
	i_data->GetItemId();
	if (context && context->m_world && !i_data->GetTargetMap().empty()) {
		context->m_world->SwitchTo(i_data->GetTargetMap());
	}
	m_systemMgr->AddEvent(i_actor, (EventId)EntityEvent::Map_Changed);
}

void S_Interaction::Interact(EventDetails *i_details) {
	if (!m_systemMgr){return;}
	if (!m_systemMgr->GetSharedContext()->m_stateManager){	return;}
	EntityManagerNew* entities = m_systemMgr->GetSharedContext()->m_entityManager;
	StateType currentState = m_systemMgr->GetSharedContext()->m_stateManager->GetCurrentState()->GetState();
	if (currentState == StateType::Game) {
		EntityId actor = FindTheClosestActor();
		if (actor == InvalidEntityId){ return;}
		if (!entities){return;}
		int player = m_systemMgr->GetSharedContext()->m_world->GetCurrentMap()->GetPlayerId();
		C_Inventory* inventory = entities->GetComponent<C_Inventory>(player,Component::Inventory);
		C_Position* actorpos = entities->GetComponent<C_Position>(actor, Component::Position);
		C_Interactable* interactable = entities->GetComponent<C_Interactable>(actor, Component::Interactable);
		if (!interactable){return;}
		InteractionType interaction = interactable->GetType();
		if (interaction == InteractionType::None){return;}
		if (!inventory) {return;}
		if (!actorpos){return;}
		if (interaction == InteractionType::AidKit|| interaction == InteractionType::PickupItem) {
			HandlePickupItem(player,actor, interactable);
			const sf::Vector2f actorPosition = actorpos->GetPosition();
			const unsigned int elevation = actorpos->getElevation();;
			const unsigned int tileX = static_cast<unsigned int>(actorPosition.x /Sheet::Tile_Size);
			const unsigned int tileY = static_cast<unsigned int>(actorPosition.y /Sheet::Tile_Size);
			Items::ItemId item = inventory->GetIdByStringId(entities->GetEntityName(actor));
			if (inventory->AddItem(item, interactable->GetQuantity())) {
				Map* map = m_systemMgr->GetSharedContext()->m_world->GetCurrentMap();
				TileMap* tiles = map ? map->GetTileMap() : nullptr;

				bool removedLinkedTile = false;

				if (tiles) {
					const sf::Vector2u mapSize = map->GetMapSize();

					if (mapSize.x > 0 && mapSize.y > 0) {
						for (const auto& [key, tile] : *tiles) {
							if (!tile || tile->m_linkedEntity != static_cast<int>(actor)) {
								continue;
							}

							// Decode the tile-map key; this matches Map::SaveTiles().
							const unsigned int x = key % mapSize.x;
							const unsigned int temp = key / mapSize.x;
							const unsigned int y = temp % mapSize.y;
							const unsigned int layer = temp / mapSize.y;

							removedLinkedTile = map->RemoveTile(x, y, layer);
							if (!map->SaveTiles()) {
								printf("tiles are not saved");
							}
							break; // RemoveTile erases this entry from the map.
						}
					}
				}

				// The actor may not be linked to a map tile.
				if (!removedLinkedTile) {
					entities->RemoveEntity(actor);
				}
			}
		}else if (interaction == InteractionType::Door ) {
			HandleOpenDoor(player, actor, interactable);
		}

	}else if (currentState == StateType::FieldTerminal) {

	}

}

EntityId& S_Interaction::FindTheClosestActor() {
	if (m_systemMgr) {
		SharedContext* context = m_systemMgr->GetSharedContext();
		EntityManagerNew* entities = context->m_entityManager;
		if (context && entities && context->m_world) {
			int player = context->m_world->GetCurrentMap()->GetPlayerId();
			C_Position* playerPos = entities->GetComponent<C_Position>(player, Component::Position);
			const sf::Vector2f playerPosition = playerPos->GetPosition();
			EntityId closestId = InvalidEntityId;
			float closestDistanceSquared = INTERACTION_RADIUS * INTERACTION_RADIUS;
			if (playerPos) {
				for (auto entity: m_entities) {
					C_Position* actorPos = entities->GetComponent<C_Position>(entity, Component::Position);
					if (actorPos) {
						const sf::Vector2f actorPosition = actorPos->GetPosition();
						const float dx = actorPosition.x - playerPosition.x;
						const float dy = actorPosition.y - playerPosition.y;
						const float distanceSquared = dx * dx + dy * dy;
						if (distanceSquared > INTERACTION_RADIUS * INTERACTION_RADIUS) {
							continue;
						}

						if (distanceSquared <= closestDistanceSquared) {
							closestDistanceSquared =  distanceSquared;
							closestId = entity;
						}
					}
				}
			}
			return closestId;
		}
	}
}

ItemManager * S_Interaction::GetItemManager() const {
	SharedContext* ctx = m_systemMgr->GetSharedContext();
	return ctx ? ctx->m_itemManager : nullptr;
}

void S_Interaction::LoadMaterials() {
	std::string path = Utils::GetWorkingDirectory() + "src//Materials/";
	for (const auto& entry : std::filesystem::directory_iterator(path)) {
		if (entry.path().extension() == ".material") {
			LoadMaterial(entry.path().stem().string());
		}
	}
}

int S_Interaction::SpawnForTile(Materials::MaterialType i_material, const sf::Vector2f& i_worldPos, unsigned int i_elevation)
{
		auto itr = m_materialToTemplate.find(Materials::MaterialToString(i_material));
		if (itr == m_materialToTemplate.end()) { return -1; }

		EntityManagerNew* entities = m_systemMgr->GetEntityManager();
		int id = entities->AddEntity(itr->second); // спавнит по .entity шаблону (например "AidKit")
		if (id == -1) { return -1; }

		// позиция из самого .entity файла (его собственный Component 0 ...) тут неважна —
		// перезаписываем её реальными координатами конкретного тайла
		C_Position* pos = entities->GetComponent<C_Position>(id, Component::Position);
		if (pos) { pos->SetPosition(i_worldPos); pos->SetElevation(i_elevation); }
		return id;
	
}

void S_Interaction::InjectCallback() {
	m_systemMgr->GetSharedContext()->m_eventManager->AddCallback(StateType(0),"Key_E",
		&S_Interaction::Interact,this);

}


void S_Interaction::LoadMaterial(const std::string& i_materialName)
{
		std::string path = Utils::GetWorkingDirectory() + "//src//Materials//" + i_materialName + ".material";
		std::ifstream file(path);
		if (!file.is_open()) { return; }
		std::string line;
		while (std::getline(file, line)) {
			std::stringstream ss(line);
			std::string key, entityTemplate;
			ss >> key;
			if (key == "Entity") {
				ss >> entityTemplate;
				m_materialToTemplate[i_materialName] = entityTemplate;
				// ничего не спауним здесь — просто запоминаем связку материал -> шаблон
		}
	}
}


	
