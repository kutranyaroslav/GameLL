#include "EntityManagerNew.h"
#include "SystemManager.h"
#include "C_SpriteSheet.h"
#include "C_Position.h"
#include "C_State.h"
#include "C_Controller.h"
#include "C_Movable.h"
#include "C_Collidable.h"
#include "C_SoundEmitter.h"
#include "C_SoundListener.h"
EntityManagerNew::EntityManagerNew(SystemManager* i_systemMgr, TextureManager* i_textureMgr):
	m_systems(i_systemMgr), m_textureManager(i_textureMgr), m_idCounter(0)
{
	AddComponentType<C_Position>(Component::Position);
	AddComponentType<C_SpriteSheet>(Component::SpriteSheet);
	AddComponentType<C_State>(Component::State);
	AddComponentType<C_Movable>(Component::Movable);
	AddComponentType<C_Controller>(Component::Controller);
	AddComponentType <C_Collidable>(Component::Collidable);
	AddComponentType<C_SoundEmitter>(Component::SoundEmitter);
	AddComponentType<C_SoundListener>(Component::SoundListener);
}

EntityManagerNew::~EntityManagerNew() { Purge(); }

int EntityManagerNew::AddEntity(const Bitmask& i_mask) {
	unsigned int entity = m_idCounter;
	if (!m_entities.emplace(entity, EntityData(0, ComponentContainer())).second) { return -1; }
	++m_idCounter;
	for (unsigned int i = 0; i < N_COMPONENT_TYPES; ++i) {
		if (i_mask.getBit(i)) { AddComponent(entity, (Component)i); }
	}
	m_systems->EntityModified(entity, i_mask);
	m_systems->AddEvent(entity, (EventId)EntityEvent::Spawned);
	return entity;
}
int EntityManagerNew::AddEntity(const std::string& i_file) {
	int EntityId = -1;
	std::ifstream file;
	file.open(Utils::GetWorkingDirectory() + "Assets/Entities/" + i_file + ".entity");
	if (!file.is_open()) {
		return -1;
	}
	std::string line;
	while (std::getline(file, line)) {
		if (line[0] == '|') { continue; }
		std::stringstream keystream(line);
		std::string type; 
		keystream >> type;
		if (type == "Name") {

		}
		else if (type == "Attributes") {
			if (EntityId != -1) { continue; }
			Bitset set = 0;
			Bitmask mask;
			keystream >> set;
			mask.setMask(set);
			EntityId = AddEntity(mask);
			if (EntityId == -1) { return -1; }
		}
		else if (type == "Component") {
			if (EntityId == -1) { continue; }
			unsigned int c_id = 0;
			keystream >> c_id;
			C_Base* component = GetComponent<C_Base>(EntityId,(Component) c_id);
			if (!component) { continue; }
			keystream >> *component;
			if (component->getComponentType() == Component::SpriteSheet) { 
				C_SpriteSheet* sheet = (C_SpriteSheet*)component;
				sheet->Create(m_textureManager);
			}
		}
 	}
	file.close();
	return EntityId;
}

bool EntityManagerNew::RemoveEntity(const EntityId& i_id) {
	auto itr = m_entities.find(i_id);
	if (itr == m_entities.end()) { return false; }
	while (itr->second.second.begin() != itr->second.second.end()) {
		delete itr->second.second.back();
		itr->second.second.pop_back();
		
	}
	m_entities.erase(itr);
	m_systems->RemoveEntity(i_id);
	return true;
}
bool EntityManagerNew::AddComponent(const EntityId& i_id, const Component& i_component) {
	auto itr = m_entities.find(i_id);
	if (itr == m_entities.end()) { return false; }
	if (itr->second.first.getBit((unsigned int)i_component)) {
		return false;
	}
	auto itr2 = m_cFactory.find(i_component);
	if (itr2 == m_cFactory.end()) { return false; }
	C_Base* component = itr2->second();
	itr->second.second.emplace_back(component);
	itr->second.first.turnOnBit((unsigned int) i_component);
	m_systems->EntityModified(i_id, itr->second.first);
	return true;

}

bool EntityManagerNew::RemoveComponent(const EntityId& i_id, const Component& i_component) {
	auto itr = m_entities.find(i_id);
	if (itr == m_entities.end()) { return false; }
	if (!itr->second.first.getBit((unsigned int)i_component)) {
		return false;
	}
	auto& container = itr->second.second;
	auto component = std::find_if(container.begin(), container.end(), [&i_component](C_Base* c ) {
		return c->getComponentType() == i_component;
		});
	if (component == container.end()) { return false; }
	delete (* component);
	container.erase(component);
	itr->second.first.clearBit((unsigned int )i_component);
	m_systems->EntityModified(i_id, itr->second.first);
	return true;

}
bool EntityManagerNew::HasComponent(const EntityId& i_id, const Component& i_component) {
	auto itr = m_entities.find(i_id);
	if (itr == m_entities.end()) { return false; }
	return itr->second.first.getBit((unsigned int)i_component);
}

void EntityManagerNew::Purge() {
	m_systems->PurgeEntities();
	for (auto& entity : m_entities) {
		for (auto& component : entity.second.second) { delete component; }
		entity.second.second.clear();
		entity.second.first.Clear();
	}
	m_entities.clear();
	m_idCounter = 0;
}

