#pragma once
#include "Directions.h"
#include "S_Base.h"
#include "C_Movable.h"
#include "C_Position.h"
enum class Axis{x,y};
class Map;
class S_Movement: public S_Base
{
public:
	S_Movement(SystemManager* i_systemMgr);
	void SetMap(Map* i_gameMap);
	void Update(float i_dT) override;
	void HandleEvent(const EntityId& i_entity, const EntityEvent& i_event) override;
	void Notify(const Message& i_message) override;
private:
	void StopEntity(const EntityId& i_entity, const Axis& i_axis);
	void SetDirection(const EntityId& i_entity, const Direction& i_dir);
	const sf::Vector2f& GetTileFriction(unsigned int i_elevation, unsigned int x, unsigned int y);
	void MovementStep(float i_dT, C_Movable* i_movable, C_Position* i_pos);
	Map* m_gameMap;
};

