#pragma once
#include "Bitmask.h"
#include "C_Base.h"
#include "C_Position.h"
#include "TextureManager.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
#include "EntityEventEnum.h"

using EntityId = unsigned int; 
using ComponentContainer = std::vector<C_Base*>;
using EntityData = std::pair<Bitmask, ComponentContainer>;
using EntityContainer = std::unordered_map<EntityId, EntityData>;
using ComponentFactory = std::unordered_map < Component, std::function<C_Base* (void)>>;

class SystemManager;
class EntityManagerNew
{
public:
	EntityManagerNew(SystemManager* i_systemMgr, TextureManager* i_textureMgr);
	~EntityManagerNew();
	int AddEntity(const Bitmask& i_mask);
	int AddEntity(const std::string& i_file);
	bool RemoveEntity(const EntityId& i_id);
	bool AddComponent(const EntityId& i_id, const Component& i_component);
	bool RemoveComponent(const EntityId& i_id, const Component& i_component);
	bool HasComponent(const EntityId& i_id, const Component& i_component);

	void Purge();
	template<class T>
	T* GetComponent(const EntityId& i_entityId, const Component& i_component) {
		auto itr = m_entities.find(i_entityId);
		if (itr == m_entities.end()) { return nullptr; }
		if (!itr->second.first.getBit((unsigned int)i_component))
		{
			return nullptr;
		}
		auto& container = itr->second.second;
		auto component = std::find_if(container.begin(), container.end(), [&i_component](C_Base* c) {
			return c->getComponentType() == i_component;
			});

		return (component != container.end() ? dynamic_cast <T*> (*component) : nullptr);
	}

private:
	template <class T>
	void AddComponentType(const Component& i_id) {
		m_cFactory[i_id] = []()->C_Base* {return new T();};
	}

	
	unsigned int m_idCounter;
	//first is the name second is an id of the entity
	std::unordered_map<std::string, EntityId> m_entitiesIdsNames;
	EntityContainer m_entities;
	ComponentFactory m_cFactory;
	SystemManager* m_systems;
	TextureManager* m_textureManager;

};

