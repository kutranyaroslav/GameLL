#include "S_Interaction.h"
#include "EntityManagerNew.h"
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


	
