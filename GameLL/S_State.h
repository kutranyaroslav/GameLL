#pragma once
#include "S_Base.h"
#include "C_State.h"
#include "Directions.h"
class S_State : public S_Base
{
public:
	S_State(SystemManager* i_systemMgr);
	void Update(float i_dT) override;
	void HandleEvent(const EntityId& i_entity, const EntityEvent& i_event) override;
	void Notify(const Message& i_message) override;
private:
	void ChangeState(const EntityId& i_entity, const EntityState& i_state, const bool& i_force);
};