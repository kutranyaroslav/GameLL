#pragma once
#include "StateManager.h"
#include "Utilitites.h"
#include "EntityManagerNew.h"
#include <fstream>
#include <unordered_map>


enum Sheet { Tile_Size = 32, Sheet_Width = 512, Sheet_Height = 256, Num_Layers = 6 };
using TileId = unsigned int;
//all the potential info for world manager

struct TileInfo {
	TileInfo(SharedContext* i_context, const std::string& i_texture = "", TileId i_id = 0, unsigned int i_row = 0) :
		m_context(i_context), m_id(0), m_deadly(false), m_row(0)
	{
		TextureManager* textureMgr = i_context->m_textureManager;
		if (i_texture == "") { m_id = i_id; return; }
		if (!textureMgr->RequireResource(i_texture)) { return; }
		m_texture = i_texture;
		m_id = i_id;
		m_row = i_row;
		m_sprite.setTexture(*textureMgr->GetResource(m_texture));
		//row must be added in second for defining top 
		sf::IntRect TileBoundaries((m_id % (Sheet::Sheet_Width / Sheet::Tile_Size) * Sheet::Tile_Size),
			m_row * Sheet::Tile_Size, Sheet::Tile_Size, Sheet::Tile_Size);
		m_sprite.setTextureRect(TileBoundaries);
	}
	~TileInfo() {
		if (m_texture == "") { return; }
		m_context->m_textureManager->ReleaseResource(m_texture);
	}
	sf::Sprite m_sprite;
	TileId m_id;
	unsigned int m_row;
	std::string m_name;
	sf::Vector2f m_friction;
	bool m_deadly;
	SharedContext* m_context;
	std::string m_texture;
};

struct Tile {
	TileInfo* m_properties;
	bool m_checkout;
	std::string m_checkoutMap;
	bool m_solid;
};
struct TileKey {
	int id; 
	int row; 
	bool operator==(const TileKey& other)const {
		return id == other.id && row == other.row;
	}
};
namespace std {
	template <>
	struct hash<TileKey> {
		std::size_t operator()(const TileKey& k) const {
			return std::hash<int>()(k.id) ^ (std::hash<int>()(k.row) << 1);
		}
	};
}

using TileMap = std::unordered_map<TileId, Tile*>;
using TileSet = std::unordered_map<TileKey, TileInfo*>;
class Map
{
public:
	Map(SharedContext* i_context,const std::string& i_mapName = "", const std::string& i_tilesetName= ""
		,const std::string& i_tileset = "", const std::string& i_texture= "");
	~Map();
	bool AddTileset(const std::string& i_name, const std::string& i_path, const std::string& i_texture);
	bool RemoveTileset(const std::string& i_name);

	bool HasTileset(const std::string& i_name);
	void LoadMap(const std::string& i_path);
	void LoadNext(const std::string& i_name);
	void Update(float i_dT);
	void Draw(unsigned int i_layer);
	
	//SETTERS AND GETTER

	void SetTileSet(const std::string& i_tileset);
	void SetTexture(const std::string& i_texture);
	int GetPlayerId();
	Tile* GetTile(unsigned int i_x, unsigned int i_y, unsigned int i_layer);
	const std::unordered_map<std::string, TileSet>& GetTilesets();
	TileInfo* GetDefaultTile();
	float GetGravity()const;
	unsigned int GetTileSize()const;
	const sf::Vector2u& GetMapSize() const;
	const sf::Vector2f& GetPlayerStart()const;
	void SetMapName(const std::string& i_name);
	std::string GetMapName();
	TileSet* GetTileset(const std::string& i_name);
private:
	unsigned int ConvertCordinates(const unsigned int& i_x, const unsigned int& i_y, const unsigned int& i_layer)const;
	bool LoadTiles(const std::string& i_path, const std::string& i_texture, TileSet& i_outTiles);
	void PurgeMap();
	void PurgeTileSet();
	

	std::string m_tilesetName;
	std::string m_texture;
	std::string m_mapName;
	std::unordered_map<std::string, TileSet> m_tilesets;
	TileMap m_tilemap;
	sf::Sprite m_background;
	TileInfo m_defaultTile;
	sf::Vector2u m_maxMapSize;
	sf::Vector2f m_playerStart;
	unsigned int m_tileCount;
	unsigned int m_tileSetCount;
	int m_playerId; 
	float m_mapGravity;
	bool m_loadNextMap;
	
	std::unordered_map<std::string, Tile*> m_checkoutTiles;
	std::string m_backgroundTexture;
	SharedContext* m_context;

};
