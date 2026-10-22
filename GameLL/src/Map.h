#pragma once
#include "StateManager.h"
#include "Utilitites.h"
#include "EntityManagerNew.h"
#include "Materials/Materials.h"
#include <fstream>
#include <unordered_map>


enum Sheet { Tile_Size = 32, Num_Layers = 6 };
using TileId = unsigned int;
//all the potential info for world manager


struct TileInfo
{
    TileInfo(
        SharedContext* i_context,
        const std::string& i_texture = "",
        TileId i_id = 0,
        unsigned int i_row = 0
    )
        : m_id(i_id),
          m_row(i_row),
          m_deadly(false),
          m_solid(false),
          m_context(i_context)
    {
        if (!m_context || i_texture.empty())
        {
            return;
        }

        TextureManager* textureMgr = m_context->m_textureManager;

        if (!textureMgr || !textureMgr->RequireResource(i_texture))
        {
            return;
        }

        m_texture = i_texture;

        sf::Texture* texture = textureMgr->GetResource(m_texture);

        if (!texture)
        {
            textureMgr->ReleaseResource(m_texture);
            m_texture.clear();
            return;
        }

        m_sprite.setTexture(*texture);

        const sf::Vector2u textureSize = texture->getSize();

        // Calculate columns using THIS texture's actual width.
        const unsigned int columns =
            textureSize.x / Sheet::Tile_Size;

        // A texture narrower than one tile cannot contain a tile.
        if (columns == 0)
        {
            textureMgr->ReleaseResource(m_texture);
            m_texture.clear();
            return;
        }

        // Preserve your existing ID + row format.
        const unsigned int column = m_id % columns;

        const unsigned int x = column * Sheet::Tile_Size;
        const unsigned int y = m_row * Sheet::Tile_Size;

        // Validate the selected tile against the actual texture.
        if (x + Sheet::Tile_Size > textureSize.x ||
            y + Sheet::Tile_Size > textureSize.y)
        {
            textureMgr->ReleaseResource(m_texture);
            m_texture.clear();
            return;
        }

        m_sprite.setTextureRect(sf::IntRect(
            static_cast<int>(x),
            static_cast<int>(y),
            Sheet::Tile_Size,
            Sheet::Tile_Size
        ));
    }

    ~TileInfo()
    {
        if (!m_texture.empty() && m_context)
        {
            m_context->m_textureManager->ReleaseResource(m_texture);
        }
    }

	sf::FloatRect GetCollisionBounds() const
	{
		return m_hasCustomCollisionBounds
			? m_collisionBounds
			: sf::FloatRect(0.f, 0.f,
				Sheet::Tile_Size, Sheet::Tile_Size);
	}
    sf::Sprite m_sprite;
    TileId m_id;
    unsigned int m_row;
    std::string m_name;
    std::string m_tilesetName;
    sf::Vector2f m_friction;
    bool m_deadly;
    SharedContext* m_context;
    std::string m_texture;
    bool m_solid;
    std::vector<Materials::MaterialType> m_materialTags;
	sf::FloatRect m_collisionBounds{ 0.f, 0.f, Sheet::Tile_Size, Sheet::Tile_Size };
	bool m_hasCustomCollisionBounds = false;
};


struct Tile {
	TileInfo* m_properties;
	bool m_checkout;
	std::string m_checkoutMap;
	int m_linkedEntity = -1;
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
	bool SaveTilesetHitbox(const std::string& i_name, const TileInfo* i_tile);

	bool HasTileset(const std::string& i_name);
	void LoadMap(const std::string& i_path);
	void LoadNext(const std::string& i_name);
	void Update(float i_dT);
	void Draw(sf::RenderTarget& i_target,const sf::View& i_view,unsigned int i_layer);
	bool SaveTiles();
	// единая точка входа "положить тайл в (x,y,layer)" — и загрузка .map, и редактор
	// идут через неё, чтобы привязанная логика (спавн сущности под материалом) срабатывала одинаково
	Tile* PlaceTile(unsigned int i_x, unsigned int i_y, unsigned int i_layer, TileInfo* i_info);
	bool RemoveTile(unsigned int i_x, unsigned int i_y, unsigned int i_layer);
	
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
	TileMap* GetTileMap();
	unsigned int ConvertCordinates(const unsigned int& i_x, const unsigned int& i_y, const unsigned int& i_layer)const;
private:
	
	// i_name - имя тайлсета, под которым он лежит в m_tilesets. Прокидывается сюда, чтобы
	// проштамповать его в каждый TileInfo: SaveTiles пишет TileInfo::m_tilesetName в строку
	// TILE, и без этого имя уезжало в файл пустым.
	bool LoadTiles(const std::string& i_name, const std::string& i_path, const std::string& i_texture, TileSet& i_outTiles);
	void PurgeMap();
	void PurgeTileSet();
	

	std::string m_tilesetName;
	std::string m_texture;
	std::string m_mapName;
	std::string m_mapFileName;
	std::unordered_map<std::string, TileSet> m_tilesets;
	std::unordered_map<std::string, std::string> m_tilesetPaths;
	TileMap m_tilemap;
	sf::Sprite m_background;
	TileInfo m_defaultTile;
	sf::Vector2u m_maxMapSize;
	sf::Vector2f m_playerStart;
	unsigned int m_tileCount;
	unsigned int m_tileSetCount;
	int m_playerId = -1;
	float m_mapGravity;
	bool m_loadNextMap;
	
	std::unordered_map<std::string, Tile*> m_checkoutTiles;
	std::string m_backgroundTexture;
	SharedContext* m_context;

};
