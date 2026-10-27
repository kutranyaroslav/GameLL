#include "Map.h"
#include "S_Collision.h"
#include "S_Movement.h"
#include "S_Interaction.h"
#include <cctype>
#include <exception>
#include <iomanip>
#include <sstream>
#include <vector>
#include <cmath>
#include <filesystem>

namespace {
	struct ConfigToken {
		std::string value;
		std::size_t begin;
		std::size_t end;
	};

	std::vector<ConfigToken> TokenizeConfigLine(const std::string& line) {
		std::vector<ConfigToken> tokens;
		std::size_t pos = 0;
		while (pos < line.size()) {
			while (pos < line.size() && std::isspace(static_cast<unsigned char>(line[pos]))) { ++pos; }
			if (pos == line.size()) { break; }
			const std::size_t begin = pos;
			while (pos < line.size() && !std::isspace(static_cast<unsigned char>(line[pos]))) { ++pos; }
			tokens.push_back({line.substr(begin, pos - begin), begin, pos});
		}
		return tokens;
	}

	bool ParseConfigFloat(const std::string& value, float& result) {
		try {
			std::size_t consumed = 0;
			result = std::stof(value, &consumed);
			return consumed == value.size() && std::isfinite(result);
		} catch (const std::exception&) { return false; }
	}

	bool ParseConfigFlag(const std::string& value, bool& result) {
		if (value == "0") { result = false; return true; }
		if (value == "1") { result = true; return true; }
		return false;
	}

	// Locate the friction/deadly/solid columns by their numeric shape. Tile names
	// are allowed to contain spaces, so they cannot be consumed as one token.
	bool FindTileConfigFields(const std::vector<ConfigToken>& tokens, std::size_t& propertyStart) {
		if (tokens.size() < 7) { return false; }
		for (std::size_t i = 2; i + 3 < tokens.size(); ++i) {
			float frictionX = 0.f, frictionY = 0.f;
			bool deadly = false, solid = false;
			if (ParseConfigFloat(tokens[i].value, frictionX) && frictionX >= 0.f && frictionX <= 1.f &&
				ParseConfigFloat(tokens[i + 1].value, frictionY) && frictionY >= 0.f && frictionY <= 1.f &&
				ParseConfigFlag(tokens[i + 2].value, deadly) && ParseConfigFlag(tokens[i + 3].value, solid)) {
				propertyStart = i;
				return true;
			}
		}
		return false;
	}
}

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
	if(!LoadTiles(i_name,i_path,i_texture,tileset)){return false;}
	auto inserted = m_tilesets.emplace(i_name, std::move(tileset));
	if (!inserted.second) { return false; }
	m_tilesetPaths[i_name] = i_path;
	return true;

}
bool Map::RemoveTileset(const std::string& i_name) {
	auto itr = m_tilesets.find(i_name);
	if (itr == m_tilesets.end()) { return false; }
	for (auto& itr2 : itr->second) {
		delete itr2.second;
	}
	itr->second.clear();
	m_tilesets.erase(itr);
	m_tilesetPaths.erase(i_name);
	return true;

}

bool Map::SaveTilesetHitbox(const std::string& i_name, const TileInfo* i_tile)
{
	if (!i_tile) { return false; }
	auto source = m_tilesetPaths.find(i_name);
	if (source == m_tilesetPaths.end()) { return false; }
	const std::string fileName = Utils::GetWorkingDirectory() + "nav//" + source->second;
	std::ifstream input(fileName);
	if (!input.is_open()) { return false; }

	std::vector<std::string> lines;
	std::string line;
	bool updated = false;
	while (std::getline(input, line)) {
		const std::vector<ConfigToken> tokens = TokenizeConfigLine(line);
		if (!tokens.empty() && tokens[0].value[0] != '|') {
			try {
				const int tileId = std::stoi(tokens[0].value);
				const int tileRow = tokens.size() > 1 ? std::stoi(tokens[1].value) : -1;
				std::size_t propertyStart = 0;
				if (tileId == static_cast<int>(i_tile->m_id) &&
					tileRow == static_cast<int>(i_tile->m_row) &&
					FindTileConfigFields(tokens, propertyStart)) {
					const std::size_t solidStart = tokens[propertyStart + 3].begin;
					const std::size_t solidEnd = tokens[propertyStart + 3].end;
					std::string suffix = line.substr(solidEnd);
					const std::vector<ConfigToken> suffixTokens = TokenizeConfigLine(suffix);
					std::size_t keepFrom = 0;
					bool hasBounds = false;
					if (!suffixTokens.empty() && ParseConfigFlag(suffixTokens[0].value, hasBounds)) {
						keepFrom = suffixTokens[0].end;
						if (hasBounds) {
							if (suffixTokens.size() < 5) { lines.push_back(line); continue; }
							keepFrom = suffixTokens[4].end;
						}
					}
					suffix = suffix.substr(keepFrom);
					std::ostringstream collision;
					collision << ' ' << (i_tile->m_hasCustomCollisionBounds ? 1 : 0);
					if (i_tile->m_hasCustomCollisionBounds) {
						collision << std::fixed << std::setprecision(3)
							<< ' ' << i_tile->m_collisionBounds.left
							<< ' ' << i_tile->m_collisionBounds.top
							<< ' ' << i_tile->m_collisionBounds.width
							<< ' ' << i_tile->m_collisionBounds.height;
					}
				line = line.substr(0, solidStart) + (i_tile->m_solid ? "1" : "0") +
					collision.str() + suffix;
					updated = true;
				}
			} catch (const std::exception&) {
				// Header/comment lines are copied unchanged.
			}
		}
		lines.push_back(line);
	}
	input.close();
	if (!updated) { return false; }

	std::ofstream output(fileName, std::ios::trunc);
	if (!output.is_open()) { return false; }
	for (const std::string& savedLine : lines) { output << savedLine << '\n'; }
	output.flush();
	const bool saved = output.good();
	output.close();
	return saved && !output.fail();
}

bool Map::HasTileset(const std::string& i_name) {
	return m_tilesets.find(i_name)!= m_tilesets.end();
}

std::string Map::GetMapName()
{
	return m_mapName;
}

TileSet* Map::GetTileset(const std::string& i_name) {
	auto found = m_tilesets.find(i_name);
	return found == m_tilesets.end() ? nullptr : &found->second;
}
TileMap* Map::GetTileMap()
{
	return &m_tilemap;
}
//all coords given to the function must be divided by tile size
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
	if (m_mapFileName.empty()) { return false; }
	//saving of the header lines of the .map file 
	std::vector<std::string> headerLines;

	std::ifstream file(m_mapFileName);
	if (!file.is_open()) { return false; }

	std::string line;
	bool hasSizeHeader = false;

	while (std::getline(file, line))
	{
		std::stringstream stream(line);

		std::string type;
		stream >> type;
		if (type == "SIZE") { hasSizeHeader = true; }

		if (type == "TILE")
			break;

		headerLines.push_back(line);
	}

	file.close();
	// An unreadable, empty, or malformed source must never be replaced by a
	// headerless map. Keep the original intact so it can be repaired manually.
	if (!hasSizeHeader || headerLines.empty()) { return false; }

	std::error_code backupError;
	std::filesystem::path backupPath(m_mapFileName);
	backupPath += ".bak";
	if (!std::filesystem::copy_file(m_mapFileName, backupPath,
		std::filesystem::copy_options::overwrite_existing, backupError) || backupError) {
		std::cerr << "Could not create map backup '" << backupPath.string()
			<< "': " << backupError.message() << '\n';
		return false;
	}


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
	return outFile.good();

}



void Map::PurgeMap() {
	m_tileCount = 0;
	for (auto& itr : m_tilemap) {
		delete itr.second;
	}
	m_tilemap.clear();
	m_context->m_entityManager->Purge();
	// Purge() destroyed the player entity, so the cached id no longer refers
	// to anything.
	m_playerId = -1;
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
	m_tilesetPaths.clear();
	m_tileSetCount = 0;
}

bool Map::LoadTiles(const std::string& i_name, const std::string& i_path, const std::string& i_texture, TileSet& i_outTiles) {
	std::ifstream file;
	file.open(Utils::GetWorkingDirectory() + "nav//"+ i_path);
	if (file.is_open()) {
		std::string line;
		while (std::getline(file, line)) {
			const std::vector<ConfigToken> tokens = TokenizeConfigLine(line);
			if (tokens.empty() || tokens[0].value[0] == '|') { continue; }
			int tileId = -1, tileRow = -1;
			try {
				tileId = std::stoi(tokens[0].value);
				tileRow = tokens.size() > 1 ? std::stoi(tokens[1].value) : -1;
			} catch (const std::exception&) { continue; }
			std::size_t propertyStart = 0;
			if (tileId < 0 || tileRow < 0 || !FindTileConfigFields(tokens, propertyStart)) { continue; }
			TileKey key{ tileId, tileRow };
			///Add texture of tile set by name Tilesheet in textures.cfg 
			TileInfo* tile = new TileInfo(m_context, i_texture, tileId, tileRow);
			// Без этого SaveTiles писал пустое имя тайлсета в строку TILE ("TILE  0 0 15 5 0 0"),
			// а LoadMap такие строки молча пропускал - карта пустела от сессии к сессии.
			tile->m_tilesetName = i_name;
			for (std::size_t nameToken = 2; nameToken < propertyStart; ++nameToken) {
				if (!tile->m_name.empty()) { tile->m_name += ' '; }
				tile->m_name += tokens[nameToken].value;
			}
			ParseConfigFloat(tokens[propertyStart].value, tile->m_friction.x);
			ParseConfigFloat(tokens[propertyStart + 1].value, tile->m_friction.y);
			ParseConfigFlag(tokens[propertyStart + 2].value, tile->m_deadly);
			ParseConfigFlag(tokens[propertyStart + 3].value, tile->m_solid);

			std::stringstream tail(line.substr(tokens[propertyStart + 3].end));
			std::string firstTailToken;
			std::string material;
			bool customAabb = false;
			if (tail >> firstTailToken) {
				bool hasAabbFlag = false;
				if (ParseConfigFlag(firstTailToken, hasAabbFlag)) {
					customAabb = hasAabbFlag;
					if (customAabb) {
						float left = 0.f, top = 0.f, width = 0.f, height = 0.f;
						if (tail >> left >> top >> width >> height) {
							tile->m_hasCustomCollisionBounds = true;
							tile->m_collisionBounds = sf::FloatRect(left, top, width, height);
						}
					}
					tail >> material;
				} else {
					material = firstTailToken;
				}
			}
			if (material != "") {
				int numofMaterialTags = 0;
				tail >> numofMaterialTags;
				for (int i = 0; i < numofMaterialTags; ++i) {
					std::string materialTag;
					//a count bigger than the tags on the line used to tag the tile with None
					if (!(tail >> materialTag)) { break; }
					Materials::MaterialType type = Materials::StringToMaterialType(materialTag);
					if (type == Materials::MaterialType::None) { continue; }
					tile->m_materialTags.push_back(type);
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
				Tile* tile = PlaceTile(tileCords.x, tileCords.y, tileLayer, itr3->second);
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
				sf::Vector2f viewSize = m_context->m_stateManager->GetCurrentState()->GetView()->getSize();
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
				if (name == "PLAYER" && m_playerId != -1) {
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
Tile* Map::PlaceTile(unsigned int i_x, unsigned int i_y, unsigned int i_layer, TileInfo* i_info) {
	if (!i_info) { return nullptr; }
	RemoveTile(i_x, i_y, i_layer); // если там уже что-то стояло — уберём вместе с его сущностью

	Tile* tile = new Tile();
	tile->m_properties = i_info;
	tile->m_checkout = false;

	for (auto& tag : i_info->m_materialTags) {
		int linked = m_context->m_systemManager->GetSystem<S_Interaction>(System::Interaction)
			->SpawnForTile(tag,
				sf::Vector2f((i_x + 0.5f) * Sheet::Tile_Size, (i_y + 0.5f) * Sheet::Tile_Size),
				i_layer);
		if (linked != -1) { tile->m_linkedEntity = linked; break; }
	}

	m_tilemap.emplace(ConvertCordinates(i_x, i_y, i_layer), tile);
	return tile;
}

bool Map::RemoveTile(unsigned int i_x, unsigned int i_y, unsigned int i_layer) {
	auto itr = m_tilemap.find(ConvertCordinates(i_x, i_y, i_layer));
	if (itr == m_tilemap.end()) { return false; }
	if (itr->second->m_linkedEntity != -1) {
		// иначе перекраска клетки в редакторе оставляет старую аптечку висеть без тайла под ней
		m_context->m_entityManager->RemoveEntity(itr->second->m_linkedEntity);
	}
	delete itr->second;
	m_tilemap.erase(itr);
	return true;
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
