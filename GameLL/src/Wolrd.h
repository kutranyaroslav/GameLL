#pragma once 
#include "Map.h"

using Maps = std::unordered_map<MapProps, Map*>;

class World {
public:
	World();
	~World();
	bool AddMap(const std::string& i_name, unsigned int i_width, unsigned int i_height);
	bool RemoveMap(const std::string& i_name);
	void Purge();
private:
	unsigned int m_idCounter; 
	Maps m_maps;
};