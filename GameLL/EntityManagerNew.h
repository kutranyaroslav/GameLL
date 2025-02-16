#pragma once
#include "Bitmask.h"
#include "C_Base.h"
#include "TextureManager.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>

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
	bool HasComponent(const EntityId& i_id, const Component& i_component);

	void Purge();

private:
	template <class T>
	void AddComponentType(const Component& i_id) {
		m_cFactory[i_id] = []()->C_Base* {return new T();};
	}
	unsigned int m_idCounter;
	EntityContainer m_entities;
	ComponentFactory m_cFactory;
	SystemManager* m_systems;
	TextureManager* m_textureManager;

};

