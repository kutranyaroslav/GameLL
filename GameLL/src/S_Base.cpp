#include "S_Base.h"


S_Base::S_Base(const System& i_id, SystemManager* i_systemMgr):
	m_id(i_id), m_systemMgr(i_systemMgr){}
S_Base::~S_Base() { Purge(); }

bool S_Base::AddEntity(const EntityId& i_entity) {
	if (HasEntity(i_entity)) { return false; }
	m_entities.emplace_back(i_entity);
	return true;
}

bool S_Base::HasEntity(const EntityId& i_entity) {	
	return std::find(m_entities.begin(), m_entities.end(), i_entity) != m_entities.end();
}
bool S_Base::RemoveEntity(const EntityId& i_entity) {
	auto itr = std::find_if(m_entities.begin(), m_entities.end(), [&i_entity](EntityId& id) {return id == i_entity;});
	if (itr == m_entities.end()) { return false; }
	m_entities.erase(itr);
	return true;
}
bool S_Base::FitsRequirements(const Bitmask& i_bits) {
	return std::find_if(m_requiredComponents.begin(), m_requiredComponents.end(), [&i_bits](Bitmask& b) {
		return b.Matches(i_bits, b.getMask());
		}) != m_requiredComponents.end();
}
void S_Base::Purge() {
	m_entities.clear();
}
System S_Base::GetId() { return m_id; }