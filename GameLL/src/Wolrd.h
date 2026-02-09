#pragma once 
#include "Map.h"

using Maps = std::unordered_map<std::string, Map*>;

class World {
public:
	World(SharedContext* i_context) ;
	~World();
	
	
	bool AddMap(const std::string& i_name, unsigned int i_width, unsigned int i_height);
	bool RemoveMap(const std::string& i_name);
	//cleans the memory
	void Purge();

private:
	unsigned int m_idCounter; 
	SharedContext * m_context;
	Maps m_maps;
};