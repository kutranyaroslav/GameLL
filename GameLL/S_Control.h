#pragma once
#include "S_Base.h"
#include "C_Movable.h"
#include "Directions.h"
class S_Control: public S_Base
{
public:
	S_Control(SystemManager* i_systemMgr);
	void HandleEvent(const EntityId& i_entity, const EntityEvent& i_event) override;
private:
	void MoveEntity(const EntityId& i_entity, const Direction& i_direction);
};

