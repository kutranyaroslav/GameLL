#include "Map.h"



Map::Map(SharedContext* i_context, BaseState* i_state):
	m_context(i_context), m_currentState(i_state), m_maxMapSize(32,32),
	m_tileCount(0), m_tileSetCount(0), m_mapGravity(512.f), m_loadNextMap(false),
	m_defaultTile(i_context)
{
	
	m_entityManager = m_context->m_entityManager;
	m_context->m_gameMap = this;
	LoadTiles("Tiles.cfg", "Tilesheet");

}
Map::~Map() {
	PurgeMap();
	PurgeTileSet();
	m_context->m_gameMap = nullptr;
}
 Tile* Map::GetTile(unsigned int i_x, unsigned int i_y, unsigned int i_layer) {
	if(i_x < 0 || i_y < 0 || i_x>= m_maxMapSize.x 
		|| i_y >= m_maxMapSize.y || i_layer < 0 || i_layer >= Sheet::Num_Layers
	)
	{
		return nullptr;
	}
	
	auto itr = m_tilemap.find(ConvertCordinates(i_x, i_y, i_layer));
	return itr != m_tilemap.end() ? itr->second : nullptr;
}

unsigned int Map::ConvertCordinates(const unsigned int& i_x, const unsigned int& i_y, const unsigned int& i_layer)const{
	return ((i_layer * m_maxMapSize.y + i_y) * m_maxMapSize.x + i_x);
}


void Map::Update(float i_dT) {
	if (m_loadNextMap) {
		PurgeMap();
		m_loadNextMap = false;
		if (m_nextMap != ""){
			LoadMap("Assets/Maps/" + m_nextMap);

		}
		else {
			m_currentState->GetStateManager()->SwitchTo(StateType::Paused);
		}
		m_nextMap = "";
	}
	sf::FloatRect viewSpace = m_context->m_wind->GetViewSpace();
	m_background.setPosition(viewSpace.left, viewSpace.top);
}

void Map::Draw(unsigned int i_layer) {
	if (i_layer >= Sheet::Num_Layers) {
		return;
	}
	sf::RenderWindow* i_wind = m_context->m_wind->GetRenderWindow();
	sf::FloatRect viewSpace = m_context->m_wind->GetViewSpace();
	sf::Vector2i tileBegin(floor(viewSpace.left / Sheet::Tile_Size), floor(viewSpace.top / Sheet::Tile_Size));
	sf::Vector2i tileEnd(ceil((viewSpace.left + viewSpace.width) / Sheet::Tile_Size),
		ceil((viewSpace.top + viewSpace.height) / Sheet::Tile_Size));
	unsigned int count = 0;
	for (int x = tileBegin.x; x <= tileEnd.x; ++x) {
		for (int y = tileBegin.y; y <= tileEnd.y; ++y) {
			if (x < 0 || y < 0) { continue; }
			Tile* tile = GetTile(x, y,i_layer);
			if (!tile) { continue; }
			if (tile->m_properties->m_id > 40) {
				std::cout << "here";
			}
			sf::Sprite& sprite = tile->m_properties->m_sprite;
			sprite.setPosition(x * Sheet::Tile_Size, y * Sheet::Tile_Size);
			i_wind->draw(sprite);
			++count;
		}
	}
}

void Map::PurgeMap() {
	m_tileCount = 0;
	for (auto& itr : m_tilemap) {
		delete itr.second;
	}
	m_tilemap.clear();
	m_entityManager->Purge();
	if (m_backgroundTexture == "") { return; }
	m_context->m_textureManager->ReleaseResource(m_backgroundTexture);
	m_backgroundTexture = "";
}
void Map::PurgeTileSet() {
	for (auto& itr : m_tileset) {
		delete itr.second;
	}
	m_tileset.clear();
	m_tileSetCount = 0;
}

void Map::LoadTiles(const std::string& i_path, const std::string& i_texture) {
	std::ifstream file;
	file.open(Utils::GetWorkingDirectory() + i_path);
	if (file.is_open()) {
		std::string line;
		while (std::getline(file, line)) {
			if (line[0] == '|') { continue; }
			std::stringstream keystream(line);
			int tileId;
			int tileRow;
			keystream >> tileId;
			keystream >> tileRow;
			TileKey key{ tileId, tileRow };
			if (tileId < 0) { continue; }
			///Add texture of tile set by name Tilesheet in textures.cfg 
			TileInfo* tile = new TileInfo(m_context, i_texture, tileId, tileRow);
			keystream >> tile->m_name >> tile->m_friction.x >> tile->m_friction.y >>
				tile->m_deadly;
			if (!m_tileset.emplace(key, tile).second) {
				std::cout << "Dublicate file \n " << std::endl;
				delete tile;
			}

			
		}
		file.close();
	}
}
void Map::LoadMap(const std::string& i_path) {
	std::ifstream file; 
	file.open(Utils::GetWorkingDirectory() + i_path);
	if (file.is_open()) {
		std::string line; 
		while (std::getline(file, line)) {
			if (line[0] == '|') { continue; }
			std::stringstream keystream(line);
			std::string type;
			keystream >> type;
			if (type == "TILE") {
				int tileId = 0;
				int row = 0;
				keystream >> tileId;
				keystream >> row;
				if (tileId < 0) { continue; }
				TileKey key{ tileId, row };
				auto itr = m_tileset.find(key);
				if (itr == m_tileset.end()) { continue; }
				sf::Vector2i tileCords;
				unsigned int tileLayer;
				unsigned int tileSolidity;
				keystream >> tileCords.x >> tileCords.y >> tileLayer >> tileSolidity;
				if (tileCords.x > m_maxMapSize.x || tileCords.y > m_maxMapSize.y || tileLayer >= Sheet::Num_Layers) { continue; }
				Tile* tile = new Tile();
				tile->m_properties = itr->second;
				tile->m_solid = tileSolidity;
				if (!m_tilemap.emplace(ConvertCordinates(tileCords.x, tileCords.y,tileLayer), tile).second) {
					delete tile;
					tile = nullptr;
					continue;
				}
				std::string warp;
				keystream >> warp;
				tile->m_warp = false;
				if (warp == "WARP") { tile->m_warp = true; }
			}
			else if(type == "BACKGROUND") {
				if (m_backgroundTexture != "") { continue; }
				keystream >> m_backgroundTexture;
				if (!m_context->m_textureManager->RequireResource(m_backgroundTexture)) {
					m_backgroundTexture = "";
					continue;
				}
				sf::Texture* texture = m_context->m_textureManager->GetResource(m_backgroundTexture);
				m_background.setTexture(*texture);
				sf::Vector2f viewSize = m_currentState->GetView().getSize();
				sf::Vector2u TextureSize = texture->getSize();
				sf::Vector2f scaleFactors;
				scaleFactors.x = viewSize.x / TextureSize.x;
				scaleFactors.y = viewSize.y / TextureSize.y;
				m_background.setScale(scaleFactors);
			}
			else if (type == "SIZE") {
				keystream >> m_maxMapSize.x >> m_maxMapSize.y;
			}
			else if (type == "GRAVITY") {
				keystream >> m_mapGravity;
			}
			else if (type == "DEFAULT_FRICTION") {
				keystream >> m_defaultTile.m_friction.x >> m_defaultTile.m_friction.y;
			}
			else if (type == "NEXTMAP") {
				keystream >> m_nextMap;
			}
			/*else if (type == "PLAYER") {
				if (m_playerId == -1) { continue; }
				m_playerId = m_entityManager->Add(EntityType::Player);
				if (m_playerId < 0) { continue; }
				float playerX = 0; float playerY = 0;
				keystream >> playerX >> playerY;
				m_entityManager->Find(m_playerId)->SetPosition(playerX, playerY);
				m_playerStart = sf::Vector2f(playerX, playerY);
			}
			else if (type == "ENEMY") {
				std::string enemyName;
				keystream >> enemyName;
				int enemyId = m_entityManager->Add(EntityType::Enemy, enemyName);
				if (enemyId < 0) { continue; }
				float enemyX = 0; float enemyY = 0;
				keystream >> enemyX >> enemyY;
				m_entityManager->Find(enemyId)->SetPosition(enemyX, enemyY);

			}*/
			else if (type == "ENTITY") {
				std::string name; 
				keystream >> name;
				if (name == "PLAYER" && m_playerId == -1) {
					continue;
				}
				int entityId = m_context->m_entityManager->AddEntity(name);
				if (entityId < 0) { continue; }
				if (name == "PLAYER") { m_playerId = entityId; }
				C_Base* position = m_context->m_entityManager->GetComponent<C_Position>(entityId, Component::Position);
				if (position) { keystream >> *position; }
			}
		}
		file.close();
	}
}

void Map::LoadNext() { m_loadNextMap = true; }


float Map::GetGravity() const { return m_mapGravity; }

TileInfo* Map::GetDefaultTile() { return &m_defaultTile; }
unsigned int Map::GetTileSize()const { return Sheet::Tile_Size; }
const sf::Vector2u& Map::GetMapSize() const { return m_maxMapSize; }
const sf::Vector2f& Map::GetPlayerStart() const { return m_playerStart; }
int Map::GetPlayerId() { return m_playerId; }