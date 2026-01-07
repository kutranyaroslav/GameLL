#pragma once
#include "Observer.h"
#include "EventQueue.h"
#include "ECS_Types.h"
#include "EntityManagerNew.h"
#include "Window.h"
class EntityManagerNew;
class S_Base;
using SystemContainer = std::unordered_map<System, S_Base*>;
using EntityEventContainer = std::unordered_map<EntityId, EventQueue>;
class SystemManager
{
public:
	SystemManager();
	~SystemManager();
	void SetEntityManager(EntityManagerNew* i_entityMgr);
	EntityManagerNew* GetEntityManager();
	MessageHandler* GetMessageHandler();
	template<class T>
	T* GetSystem(const System& i_system){
		auto itr = m_systems.find(i_system);
		return(itr != m_systems.end() ? dynamic_cast<T*>(itr->second) : nullptr);
	}
	void AddEvent(const EntityId& i_entity, const EventId& i_event);
	void Update(float i_dT);
	void HandleEvents();
	void ProccesLastEvent(S_Base* i_system, const EntityId& i_entity);
	void Draw(Window* i_wind, unsigned int i_elevation);
	void EntityModified(const EntityId& i_entity, const Bitmask& i_bits);
	void RemoveEntity(const EntityId& i_entity);
	void PurgeEntities();
	void PurgeSystems();
private:
	SystemContainer m_systems;
	EntityManagerNew* m_entityManager;
	EntityEventContainer m_entityEvents;
	MessageHandler m_messages;
};

