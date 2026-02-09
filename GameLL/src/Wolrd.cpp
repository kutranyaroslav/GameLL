#include "Wolrd.h"


World::World(SharedContext* i_context):m_context(i_context)
{
}
World::~World() {
	Purge();
}

bool World::AddMap(const std::string& i_name, unsigned int i_width, unsigned int i_height)
{
	return false;
}



void World::Purge() {
	for (auto& itr : m_maps) {
		delete itr.second;
		itr.second = nullptr;
	}
}

