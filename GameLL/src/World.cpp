#include "World.h"
#include <filesystem>
World::World(SharedContext* i_context) :m_context(i_context), m_currentMap(nullptr)
{
	std::string path = Utils::GetWorkingDirectory() + "Assets/maps/";
	for (const auto& entry : std::filesystem::directory_iterator(path)) {
		if(entry.path().extension() == ".map"){
			AddMap(entry.path().stem().string());
		}
	}
}
World::~World() {
	Purge();
}

bool World::AddMap(const std::string& i_name,const std::string& i_tilesetName ,
	const std::string& i_tileset, const std::string& i_texture)
{
	auto itr = m_maps.find(i_name);
	if (itr != m_maps.end() && !itr->second->HasTileset(i_tileset)) { 
		AddTileset(i_name, i_tilesetName, i_tileset, i_texture);
		return false; 
	}
	Map* map = new Map(m_context,i_name ,i_tilesetName,i_tileset, i_texture);
	if (!map) { return false; }
	m_maps.emplace(i_name, map);
	return true;
}

bool World::RemoveMap(const std::string& i_name)
{
	auto itr = m_maps.find(i_name);
	if (itr == m_maps.end()) { return false; }
	delete itr->second;
	m_maps.erase(itr);
	return true;
}
bool World::HasMap(const std::string& i_name) {
	return m_maps.find(i_name) != m_maps.end();
}

bool World::LoadMap(const std::string& i_name) {
	auto itr = m_maps.find(i_name);
	if (itr == m_maps.end()) { return false; }
	if (!itr->second) { return false; }
	// LoadNext purges the previous map first. LoadMap on its own appends to
	// whatever is already loaded, so entering a state that loads a map that is
	// already in memory added every ENTITY a second time.
	itr->second->LoadNext(i_name + ".map");
	m_currentMap = itr->second;
	return true;

}
//to do the logic for switching between the maps
bool World::SwitchTo(const std::string& i_name, const std::string& i_tileset, const std::string& i_texture)
{
	if (m_maps.empty()) { return false; }
	if (m_maps.find(i_name) == m_maps.end()){ return false;}
	if (!HasMap(i_name)) { return false; }
	if (GetCurrentMap()->GetMapName() == i_name) { return false; }
	m_currentMap = GetMap(i_name);
	GetCurrentMap()->LoadNext(i_name + ".map");
	return true;
}

bool World::AddTileset(const std::string& i_mapName, const std::string& i_tilesetName, const std::string& i_path, const std::string& i_texture)
{
	if (!HasMap(i_mapName)) { return false; }
	Map* map = GetMap(i_mapName);
	return map->AddTileset(i_tilesetName, i_path, i_texture);
}

bool World::RemoveTileset(const std::string& i_mapName, const std::string& i_tilesetName){
	if (!HasMap(i_mapName)) { return false; }
	Map* map = GetMap(i_mapName);
	return map->RemoveTileset(i_tilesetName);
}

bool World::HasTileset(const std::string& i_mapName, const std::string& i_tilesetName)
{
	if (!HasMap(i_mapName)) { return false; }
	Map* map = GetMap(i_mapName);
	return map->HasTileset(i_tilesetName);
}


Map* World::GetMap(const std::string& i_name) {
	auto itr = m_maps.find(i_name);
	if (itr == m_maps.end()) { return nullptr; }
	return itr->second;
}
Map* World::GetCurrentMap() { return m_currentMap; }

Maps& World::GetMaps()
{
	return m_maps;
}

TileSet* World::GetTileset(const std::string& i_mapName, const std::string& i_tilesetName) {
	if (!HasMap(i_mapName)) { return nullptr; }
	Map* map = GetMap(i_mapName);
	return map->GetTileset(i_tilesetName);
}

void World::Purge() {
	for (auto& itr : m_maps) {
		delete itr.second;
		itr.second = nullptr;
	}
}


void World::Update(float i_dT) {	
	if (m_context->m_stateManager->GetCurrentState()->GetState() == StateType::Game ||
		m_context->m_stateManager->GetCurrentState()->GetState() == StateType::Developement) {
		if (m_currentMap) { m_currentMap->Update(i_dT); }
	}
}

void World::Draw(sf::RenderTarget& i_target,const sf::View& i_view,unsigned int i_layer) {
	if (m_context->m_stateManager->GetCurrentState()->GetState() == StateType::Game ||
		m_context->m_stateManager->GetCurrentState()->GetState() == StateType::Developement) {
		if (m_currentMap) { m_currentMap->Draw(i_target,i_view,i_layer); }
	}
}