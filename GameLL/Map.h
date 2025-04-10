#pragma once
#include "StateManager.h"
#include "unordered_map"
#include "Utilitites.h"
#include "EntityManagerNew.h"
#include <fstream>



enum Sheet { Tile_Size = 32, Sheet_Width = 512, Sheet_Height = 256, Num_Layers = 3 };
using TileId = unsigned int;

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
	bool m_warp;
	bool m_solid;
};


using TileMap = std::unordered_map<TileId, Tile*>;
using TileSet = std::unordered_map<TileId, TileInfo*>;
class Map
{
public:
	Map(SharedContext* i_context, BaseState* i_currentState);
	~Map();
	Tile* GetTile(unsigned int i_x, unsigned int i_y, unsigned int i_layer);
	TileInfo* GetDefaultTile();
	float GetGravity()const;
	unsigned int GetTileSize()const;
	const sf::Vector2u& GetMapSize() const;
	const sf::Vector2f& GetPlayerStart()const;
	void LoadMap(const std::string& i_path);
	void LoadNext();
	void Update(float i_dT);
	void Draw(unsigned int i_layer);
private:
	unsigned int ConvertCordinates(const unsigned int& i_x, const unsigned int& i_y, const unsigned int& i_layer)const;
	void LoadTiles(const std::string& i_path, const std::string& i_texture);
	void PurgeMap();
	void PurgeTileSet();

	TileSet m_tileset;
	TileMap m_tilemap;
	sf::Sprite m_background;
	TileInfo m_defaultTile;
	sf::Vector2u m_maxMapSize;
	sf::Vector2f m_playerStart;
	unsigned int m_tileCount;
	unsigned int m_tileSetCount;
	unsigned int m_playerId;
	float m_mapGravity;
	std::string m_nextMap;
	bool m_loadNextMap;
	std::string m_backgroundTexture;
	BaseState* m_currentState;
	SharedContext* m_context;
	EntityManagerNew* m_entityManager;

};
