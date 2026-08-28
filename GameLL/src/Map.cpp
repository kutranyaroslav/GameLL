#include "Map.h"
#include "S_Collision.h"
#include "S_Movement.h"

Map::Map(SharedContext* i_context, const std::string& i_mapName, const std::string& i_tilesetName
	, const std::string& i_tileset, const std::string& i_texture ) :
	m_context(i_context), m_maxMapSize(32, 32),
	m_tileCount(0), m_tileSetCount(0), m_mapGravity(512.f), m_loadNextMap(false),
	m_defaultTile(i_context), m_tilesetName(i_tileset), m_texture(i_texture), m_mapName(i_mapName)
{
	if (i_tileset != "" && i_texture != "" && i_tilesetName != "") {
		AddTileset(i_tilesetName, i_tileset, i_texture);
	}
}
Map::~Map() {
	SaveTiles();
	PurgeMap();
	PurgeTileSet();

}
bool Map::AddTileset(const std::string& i_name, const std::string& i_path, const std::string& i_texture)
{	
	auto itr = m_tilesets.find(i_name);
	if (itr != m_tilesets.end()) { return false; }
	TileSet tileset;
	if(!LoadTiles(i_path,i_texture,tileset)){return false;}
	return m_tilesets.emplace(i_name, std::move(tileset)).second;

}
bool Map::RemoveTileset(const std::string& i_name) {
	auto itr = m_tilesets.find(i_name);
	if (itr == m_tilesets.end()) { return false; }
	for (auto& itr2 : itr->second) {
		delete itr2.second;
	}
	itr->second.clear();
	m_tilesets.erase(itr);
	return true;

}
bool Map::HasTileset(const std::string& i_name) {
	return m_tilesets.find(i_name)!= m_tilesets.end();
}

std::string Map::GetMapName()
{
	return m_mapName;
}

TileSet* Map::GetTileset(const std::string& i_name) {
	return &m_tilesets.find(i_name)->second;
}
TileMap* Map::GetTileMap()
{
	return &m_tilemap;
}
 Tile* Map::GetTile(unsigned int i_x, unsigned int i_y, unsigned int i_layer) {
	if(i_x < 0 || i_y < 0 || i_x>= m_maxMapSize.x 
		|| i_y >= m_maxMapSize.y || i_layer < 0 || i_layer >= Sheet::Num_Layers
	)
	{
		return nullptr;
	}
	
	auto itr = m_tilemap.find(ConvertCordinates(i_x, i_y, i_layer));
	//if (itr->second->m_properties->m_name == "EmptyBlock") { __debugbreak(); }
	return itr != m_tilemap.end() ? itr->second : nullptr;
}

 const std::unordered_map<std::string, TileSet>& Map::GetTilesets()
 {
	 return m_tilesets;
 }

unsigned int Map::ConvertCordinates(const unsigned int& i_x, const unsigned int& i_y, const unsigned int& i_layer)const{
	return ((i_layer * m_maxMapSize.y + i_y) * m_maxMapSize.x + i_x);
}


void Map::Update(float i_dT) {
	sf::FloatRect viewSpace = m_context->m_wind->GetViewSpace();
	m_background.setPosition(viewSpace.left, viewSpace.top);
}

void Map::Draw(sf::RenderTarget& i_target, const sf::View& i_view,unsigned int i_layer) {
	if (i_layer >= Sheet::Num_Layers) {
		return;
	}
	sf::Vector2f center = i_view.getCenter();
	sf::Vector2f size = i_view.getSize();
	sf::FloatRect viewSpace(center.x - size.x *0.5f, center.y - size.y *0.5f, size.x, size.y);

	sf::Vector2i tileBegin(floor(viewSpace.left/ Sheet::Tile_Size), floor(viewSpace.top / Sheet::Tile_Size));
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
			i_target.draw(sprite);
			++count;
		}
	}
}

bool Map::SaveTiles()
{
	//saving of the header lines of the .map file 
	std::vector<std::string> headerLines;

	std::ifstream file(m_mapFileName);

	std::string line;

	while (std::getline(file, line))
	{
		std::stringstream stream(line);

		std::string type;
		stream >> type;

		if (type == "TILE")
			break;

		headerLines.push_back(line);
	}

	file.close();


	std::ofstream outFile(m_mapFileName, std::ios::trunc);
	if (!outFile.is_open())
		return false;
	for (const std::string& line : headerLines)
	{
		outFile << line << '\n';
	}
	//saving of the tiles 

	for (auto const& pair : m_tilemap) {
		unsigned int key = pair.first;
		Tile* tile = pair.second;
		//decoding of the key
		unsigned int x = key % m_maxMapSize.x;

		unsigned int temp = key / m_maxMapSize.x;

		unsigned int y = temp % m_maxMapSize.y;

		unsigned int layer = temp / m_maxMapSize.y;

		outFile
			<< "TILE "
			<< tile->m_properties->m_tilesetName << ' '
			<< tile->m_properties->m_id << ' '
			<< tile->m_properties->m_row << ' '
			<< x << ' '
			<< y << ' '
			<< layer << ' '
			<< tile->m_properties->m_solid;
		if (tile->m_checkout)
		{
			outFile << " CHECKOUT "
				<< tile->m_checkoutMap;
		}
		outFile << '\n';

	}
	outFile.close();
	return true;

}



void Map::PurgeMap() {
	m_tileCount = 0;
	for (auto& itr : m_tilemap) {
		delete itr.second;
	}
	m_tilemap.clear();
	m_context->m_entityManager->Purge();
	if (m_backgroundTexture == "") { return; }
	m_context->m_textureManager->ReleaseResource(m_backgroundTexture);
	m_backgroundTexture = "";
}
void Map::PurgeTileSet() {
	for (auto& itr : m_tilesets) {
		TileSet& itr2 = itr.second;
		for (auto& tilepair : itr2) {
			delete tilepair.second;
		}
		itr2.clear();
	}
	m_tilesets.clear();
	m_tileSetCount = 0;
}

bool Map::LoadTiles(const std::string& i_path, const std::string& i_texture, TileSet& i_outTiles) {
	std::ifstream file;
	file.open(Utils::GetWorkingDirectory() + "nav//"+ i_path);
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
			std::string material = "";
			///Add texture of tile set by name Tilesheet in textures.cfg 
			TileInfo* tile = new TileInfo(m_context, i_texture, tileId, tileRow);
			keystream >> tile->m_name >> tile->m_friction.x >> tile->m_friction.y >>
				tile->m_deadly >> tile->m_solid;
			keystream >> material;
			if (material != "") {
				int numofMaterialTags = 0; 
				keystream >> numofMaterialTags;
				for (int i = 0; i < numofMaterialTags; ++i) {
					std::string materialTag;
					keystream >> materialTag;	
					tile->m_materialTags.push_back(Materials::StringToMaterialType(materialTag));
				}
			}

			if (!i_outTiles.emplace(key, tile).second) {
				std::cout << "Dublicate file \n " << std::endl;
				delete tile;
			}
		}
		file.close();
		return true;
	}
	else {
		return false;
	}
}
void Map::LoadMap(const std::string& i_path) {
	if (m_tilesets.empty()) { return; }
	std::ifstream file; 
	file.open(Utils::GetWorkingDirectory()+ "Assets//Maps//" + i_path);
	m_mapFileName = Utils::GetWorkingDirectory() + "Assets//Maps//" + i_path;
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
				std::string tilesetname;
				keystream >> tilesetname;
				keystream >> tileId;
				keystream >> row;
				if (tileId < 0) { continue; }
				TileKey key{ tileId, row };
				auto itr = m_tilesets.find(tilesetname);
				if (itr == m_tilesets.end()) { continue; }
				TileSet& itr2 = itr->second;
				auto itr3 = itr2.find(key);
				if (itr3 == itr2.end()) { continue; }
				sf::Vector2i tileCords;
				unsigned int tileLayer;
				unsigned int tileSolidity;
				keystream >> tileCords.x >> tileCords.y >> tileLayer >> tileSolidity;
				if (tileCords.x > m_maxMapSize.x || tileCords.y > m_maxMapSize.y || tileLayer >= Sheet::Num_Layers) { continue; }
				Tile* tile = new Tile();
				tile->m_properties = itr3->second;
				tile->m_properties->m_tilesetName = tilesetname;
				if (!m_tilemap.emplace(ConvertCordinates(tileCords.x, tileCords.y,tileLayer), tile).second) {
					delete tile;
					tile = nullptr;
					continue;
				}
				std::string warp;
				keystream >> warp;
				tile->m_checkout = false;
				if (warp == "CHECKOUT") { 
					std::string nextmap;
					keystream >> nextmap;
					tile->m_checkout = true;
					tile->m_checkoutMap = nextmap;
					m_checkoutTiles.emplace(std::make_pair(nextmap, tile));
				}
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
				sf::Vector2f viewSize = m_context->m_stateManager->GetCurrentState()->GetView().getSize();
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

		m_context->m_stateManager->GetSharedContext()->m_systemManager->GetSystem<S_Movement>(System::Movement)->SetMap(this);
		m_context->m_stateManager->GetSharedContext()->m_systemManager->GetSystem<S_Collision>(System::Collision)->SetMap(this);
		file.close();

	}
	
}

void Map::LoadNext(const std::string& i_name) { 
	PurgeMap();
	m_checkoutTiles.clear();
	LoadMap(i_name);
}


float Map::GetGravity() const { return m_mapGravity; }

TileInfo* Map::GetDefaultTile() { return &m_defaultTile; }
unsigned int Map::GetTileSize()const { return Sheet::Tile_Size; }
const sf::Vector2u& Map::GetMapSize() const { return m_maxMapSize; }
const sf::Vector2f& Map::GetPlayerStart() const { return m_playerStart; }
void Map::SetMapName(const std::string& i_name)
{
	m_mapName = i_name;
}
void Map::SetTileSet(const std::string& i_tileset)
{
	m_tilesetName = i_tileset;
}
void Map::SetTexture(const std::string& i_texture)
{
	m_texture = i_texture;
}
int Map::GetPlayerId() { return m_playerId; }
