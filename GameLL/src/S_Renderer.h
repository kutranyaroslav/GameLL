#pragma once
#include "S_Base.h"
#include "Window.h"
#include "Directions.h"

#include "C_SpriteSheet.h"
class S_Renderer: public S_Base
{
public:
	S_Renderer(SystemManager* i_systemMgr);
	~S_Renderer();
	void Update(float i_dT) override;
	void HandleEvent(const EntityId& i_entity, const EntityEvent& i_event) override;
	void Notify(const  Message& i_message);
	void Render(Window* i_wind, unsigned int i_layer);
private:
	void SetSheetDirection(const EntityId& i_entity, const Direction& i_dir);
	void SortDrawables();
};

