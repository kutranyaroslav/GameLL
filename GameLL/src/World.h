#pragma once
#pragma once 
#include "Map.h"

using Maps = std::unordered_map<std::string, Map*>;

class World {
public:
	World(SharedContext* i_context);
	~World();

	//emplaces the map in maps second parameter is the config lays in hard precoded dir //nav//
	bool AddMap(const std::string& i_name,const std::string& i_tilesetName = "",
		const std::string& i_tileset = "", const std::string& i_texture = "");
	bool RemoveMap(const std::string& i_name);
	bool HasMap(const std::string& i_name);
	bool LoadMap(const std::string& i_name);
	bool LoadNext(const std::string& i_name, const std::string&i_tileset = "", const std::string& i_texture = "");
	bool AddTileset(const std::string& i_mapName, const std::string& i_tilesetName,
		const std::string& i_path, const std::string& i_texture);
	bool RemoveTileset(const std::string& i_mapName, const std::string& i_tilesetName);
	bool HasTileset(const std::string& i_mapName, const std::string& i_tilesetName);
	Map* GetMap(const std::string& i_name);
	Map* GetCurrentMap();
	TileSet* GetTileset(const std::string& i_mapName, const std::string& i_tilesetName);

	void Update(float i_dT);
	void Draw(unsigned int i_layer);
	//cleans the memory
	void Purge();

private:
	SharedContext* m_context;
	Maps m_maps;
	Map* m_currentMap;
};