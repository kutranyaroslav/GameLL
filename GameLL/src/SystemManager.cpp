#include "SystemManager.h"
#include "EntityManagerNew.h"
#include "S_Base.h"
#include "S_Movement.h"
#include "S_Renderer.h"
#include "S_Control.h"
#include "S_SheetAnimation.h"
#include "S_State.h"
#include "S_Collision.h"
#include "S_Sound.h"
#include "S_Lighting.h"
#include "S_Particles.h"
#include "S_Interaction.h"
SystemManager::SystemManager() :m_entityManager(nullptr){
	m_systems[System::Control] = new S_Control(this);
	m_systems[System::Movement] = new S_Movement(this);
	m_systems[System::State] = new S_State(this);
	m_systems[System::Renderer] = new S_Renderer(this);
	m_systems[System::Collision] = new S_Collision(this);
	m_systems[System::SheetAnimation] = new S_SheetAnimation(this);
	m_systems[System::Sound] = new S_Sound(this);
	m_systems[System::Lighting] = new S_Lighting(this);
	m_systems[System::Particles] = new S_Particles(this);
	m_systems[System::Interaction] = new S_Interaction(this);

}
SystemManager::~SystemManager() { PurgeSystems(); }

void SystemManager::SetEntityManager(EntityManagerNew* i_entityMgr) {
	if (!m_entityManager) {
		m_entityManager = i_entityMgr;
	}
}
void SystemManager::SetSharedContext(SharedContext* i_context)
{
	if (!m_context) {
		m_context = i_context;
	}
}
SharedContext* SystemManager::GetSharedContext()
{
	return m_context;
}
EntityManagerNew* SystemManager::GetEntityManager() { return m_entityManager; }
MessageHandler* SystemManager::GetMessageHandler() { return &m_messages; }

void SystemManager::AddEvent(const EntityId& i_entity, const EventId& i_event) {
	m_entityEvents[i_entity].AddEvent(i_event);
}

void SystemManager::Update(float i_dT) {
	for (auto& itr : m_systems) {
		itr.second->Update(i_dT);
	}
    HandleEvents();
	m_messages.ProcessQueuedMessages();
}
void SystemManager::HandleEvents() {
	for (auto& event : m_entityEvents)
	{
		EventId id = 0;
		while (event.second.ProcessEvents(id)) {
			for (auto& system : m_systems) {
				if (system.second->HasEntity(event.first)) {
					system.second->HandleEvent(event.first, static_cast<EntityEvent> (id));
				}
			}
		}
	}
}

void SystemManager::Draw(Window* i_wind, unsigned int i_elevation) {
	auto itr = m_systems.find(System::Renderer);
	if (itr == m_systems.end()) { return; }
	S_Renderer* system = (S_Renderer*)itr->second;
	system->Render(i_wind,i_elevation);

}
// Separate from Draw: the light map is built once per frame, not once per
// elevation layer.
void SystemManager::DrawLighting(Window* i_wind) {
	auto itr = m_systems.find(System::Lighting);
	if (itr == m_systems.end()) { return; }
	((S_Lighting*)itr->second)->Render(i_wind);
}
// Like DrawLighting: once per frame, not once per elevation layer.
void SystemManager::DrawParticles(Window* i_wind) {
	auto itr = m_systems.find(System::Particles);
	if (itr == m_systems.end()) { return; }
	((S_Particles*)itr->second)->Render(i_wind);
}
//here we check entity for suitable components and then add it to the systems
void SystemManager::EntityModified(const EntityId& i_entity, const Bitmask& i_bits) {
	for (auto& itr : m_systems) {
		S_Base* system = itr.second;
		//if (itr.first == System::Sound) { __debugbreak(); }
		if (system->FitsRequirements(i_bits)) {
			if (!system->HasEntity(i_entity)) {
				system->AddEntity(i_entity);
			}
		}
		else {
			if (system->HasEntity(i_entity)) {
				system->RemoveEntity(i_entity);
			}
		}
	}
}

void SystemManager::RemoveEntity(const EntityId& i_entity) {
	for (auto& system : m_systems) {
		system.second->RemoveEntity(i_entity);
	}
}

void SystemManager::PurgeEntities() {
	for (auto& system : m_systems) {
		system.second->Purge();
	}
}

void SystemManager::PurgeSystems() {
	for (auto& system : m_systems) {
		delete system.second;
	}
	m_systems.clear();
}

void SystemManager::ProccesLastEvent(S_Base* i_system, const EntityId& i_entity) {
	EventId i_event = 0;
	if (m_entityEvents[i_entity].ProcessEvents(i_event)) {
		i_system->HandleEvent(i_entity, static_cast<EntityEvent> (i_event));
	}
	
}