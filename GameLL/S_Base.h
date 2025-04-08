#pragma once
#include <vector>
#include "EntityManagerNew.h"
#include "SystemManager.h"
#include "EventQueue.h"
#include "Observer.h"
using EntityList = std::vector<EntityId>; 
using Requirements = std::vector<Bitmask>;

class S_Base: Observer
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
	virtual void HandleEvent(const EntityId& i_entity, const EntityEvent& i_event) = 0;
protected:
	System m_id;
	Requirements m_requiredComponents;
	EntityList m_entities;
	SystemManager* m_systemMgr;
};

