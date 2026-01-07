#pragma once
#include "S_Base.h"
#include "C_Collidable.h"
#include "Map.h"
#include <vector>
struct CollisionElement {
	CollisionElement(float i_area, TileInfo* i_info, const sf::FloatRect& i_bounds) :
	m_area(i_area),m_tile(i_info), m_tileBounds(i_bounds) {}
	float m_area;
	TileInfo* m_tile;
	sf::FloatRect m_tileBounds;
};
using Collisions = std::vector<CollisionElement>;
class S_Collision:public S_Base
{
public:
	S_Collision(SystemManager* i_systemMgr);
	void SetMap(Map* i_map);
	Map* GetMap();
	void Update(float i_dT) override;
	void Notify(const Message& i_message) override;
	void HandleEvent(const EntityId& i_entity, const EntityEvent& i_event)override;
private:
	void CheckOutOfBounds(C_Position* i_pos, C_Collidable* i_collidable);
	void MapCollisions(const EntityId& i_entity,C_Position* i_pos, C_Collidable* i_collidable);
	void EntityCollisions();

	Map* m_gameMap;
};

