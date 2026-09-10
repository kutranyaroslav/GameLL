#include "S_Interaction.h"
#include "EntityManagerNew.h"
#include "StateManager.h" // for SharedContext (m_world, m_textbox)
#include "World.h"
#include "Textbox.h"
#include <filesystem>
S_Interaction::S_Interaction(SystemManager* i_systemMgr) :
	S_Base(System::Interaction, i_systemMgr)
{
	Bitmask req;
	req.turnOnBit((unsigned int)Component::Position);
	req.turnOnBit((unsigned int)Component::SpriteSheet);
	req.turnOnBit((unsigned int)Component::State);
	req.turnOnBit((unsigned int)Component::Collidable);
	req.turnOnBit((unsigned int)Component::Interactable);
	m_requiredComponents.push_back(req);
	req.Clear();

	m_systemMgr->GetMessageHandler()->Subscribe(EntityMessage::Interaction_Action, this);
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

void S_Interaction::Notify(const Message& i_message) {
	if ((EntityMessage)i_message.m_type != EntityMessage::Interaction_Action) { return; }
	if (!HasEntity(i_message.m_receiver)) { return; } // цель не интерактивна — нет нужных компонентов

	EntityManagerNew* entities = m_systemMgr->GetEntityManager();
	C_Interactable* data = entities->GetComponent<C_Interactable>(i_message.m_receiver, Component::Interactable);
	if (!data || (data->IsOneShot() && data->IsUsed())) { return; }

	auto handler = m_handlers.find(data->GetType());
	if (handler == m_handlers.end()) { return; }
	handler->second(i_message.m_receiver, i_message.m_sender, data);

	if (data->IsOneShot()) { data->SetUsed(true); }

	Message done((MessageType)EntityMessage::Interaction_Completed);
	done.m_sender = i_message.m_sender;
	done.m_receiver = i_message.m_receiver;
	m_systemMgr->GetMessageHandler()->Dispatch(done);
}

void S_Interaction::HandleOpenDoor(EntityId i_target, EntityId i_actor, C_Interactable* i_data) {
	// тут конкретная механика двери — переключить solid у C_Collidable, сменить кадр спрайта и т.п.
	m_systemMgr->AddEvent(i_target, (EventId)EntityEvent::Door_Open);
}

void S_Interaction::HandleHeal(EntityId i_target, EntityId i_actor, C_Interactable* i_data) {
	// у тебя пока нет C_Health в ECS_Types — как появится, тут будет:
	// entities->GetComponent<C_Health>(i_actor, Component::Health)->Add(i_data->GetAmount());
	m_systemMgr->AddEvent(i_actor, (EventId)EntityEvent::Healed);
}

void S_Interaction::HandlePickupItem(EntityId i_target, EntityId i_actor, C_Interactable* i_data) {
	// нет C_Inventory / InventoryManager пока — когда появится:
	// inventory->AddItem(i_data->GetItemId(), i_data->GetQuantity());
	// m_oneShot всегда true для PickupItem (см. C_Interactable::ReadIn), так что Notify()
	// сам выставит m_used = true после этого вызова и повторно предмет не сработает.
	// Настоящий despawn (EntityManagerNew::RemoveEntity) сюда нельзя добавлять как есть:
	// Notify() трогает i_data сразу после возврата отсюда, а RemoveEntity удаляет компоненты -
	// это UAF. Когда появится настоящий деспавн/пул сущностей, убирать нужно будет отложенно
	// (следующий кадр), а не прямо из хендлера.
	m_systemMgr->AddEvent(i_actor, (EventId)EntityEvent::Item_Picked);
}

void S_Interaction::HandleReadNote(EntityId i_target, EntityId i_actor, C_Interactable* i_data) {
	SharedContext* context = m_systemMgr->GetSharedContext();
	if (context && context->m_textbox) {
		// GetTextId() сейчас просто id/ключ — как появится текстовый/локализационный менеджер,
		// тут будет context->m_textbox->Add(localization->Get(i_data->GetTextId()));
		context->m_textbox->Add(i_data->GetTextId());
	}
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
	SharedContext* context = m_systemMgr->GetSharedContext();
	if (context && context->m_textbox) {
		context->m_textbox->Add(i_data->GetTextId());
	}
	m_systemMgr->AddEvent(i_target, (EventId)EntityEvent::Examined);
}

void S_Interaction::HandleSavePoint(EntityId i_target, EntityId i_actor, C_Interactable* i_data) {
	// нет SaveManager пока — как появится:
	// m_systemMgr->GetSharedContext()->m_saveManager->Save();
	m_systemMgr->AddEvent(i_target, (EventId)EntityEvent::Game_Saved);
}

void S_Interaction::HandleTerminal(EntityId i_target, EntityId i_actor, C_Interactable* i_data) {
	SharedContext* context = m_systemMgr->GetSharedContext();
	if (context && context->m_textbox) {
		context->m_textbox->Add(i_data->GetTextId());
	}
	m_systemMgr->AddEvent(i_target, (EventId)EntityEvent::Terminal_Used);
}

void S_Interaction::HandleElevator(EntityId i_target, EntityId i_actor, C_Interactable* i_data) {
	SharedContext* context = m_systemMgr->GetSharedContext();
	if (context && context->m_world && !i_data->GetTargetMap().empty()) {
		context->m_world->SwitchTo(i_data->GetTargetMap());
	}
	m_systemMgr->AddEvent(i_actor, (EventId)EntityEvent::Map_Changed);
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


	
