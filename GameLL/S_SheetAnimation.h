#pragma once
#include "S_Base.h"
#include "C_SpriteSheet.h"
#include "C_State.h"
class S_SheetAnimation: public S_Base
{
public:
	S_SheetAnimation(SystemManager* i_systemMgr);
	void Update(float i_dT)override;
	void Notify(const Message& i_message) override;
	void HandleEvent(const EntityId& i_entity, const EntityEvent& i_event)override;
private:
	void ChangeAnimation(const EntityId& i_entity, const std::string& i_anim, bool i_play, bool i_loop);
};

