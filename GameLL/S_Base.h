#pragma once
#include <vector>
#include "EntityManagerNew.h"
#include "EventQueue.h"

using EntityList = std::vector<EntityId>; 
using Requirements = std::vector<Bitmask>;
class SystemManager;
class S_Base
{
public: 
	S_Base(const System& i_id, SystemManager* i_systemMgr);
	virtual ~S_Base();
	bool AddEntity(const EntityId& i_entity);
	bool HasEntity(const EntityId& i_entity);
	bool RemoveEntity(const EntityId& i_entity);
	System GetId();
	bool FitsRequirements(const Bitmask& i_bits);
	void Purge();
	virtual void Update(float i_dT) = 0;
	virtual void HandleEvent(const EntityId& i_entity, const EventId& i_event);
protected:
	System m_id;
	Requirements m_requiredComponents;
	EntityList m_entities;
	SystemManager* m_systemMgr;
};

