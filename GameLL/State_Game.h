#pragma once
#include "StateManager.h"
#include "SpriteSheet.h"
#include "Anim_Directional.h"
#include "Map.h"
#include "C_Position.h"


class State_Game:public BaseState
{
public:
	State_Game(StateManager* i_stateManager);
	void onCreate() override;
	void onDestroy() override;
	void Activate() override;
	void Deactivate() override;
	void Draw() override;
	void Update(const sf::Time& i_time) override;
	void MainMenu(EventDetails* i_details);
	void Pause(EventDetails* i_details);
	//Test integration of animations
	void Cast(EventDetails* i_details);
	
	void Move(EventDetails* i_details);
private:
	void UpdateCamera();
	sf::Texture m_texture;
	sf::Sprite m_sprite;
	sf::Vector2f m_increment;
	TextureManager m_textureMgr;
	Anim_Directional m_anim;
	SpriteSheet m_spriteSheet;
	Map* m_testMap;
	int m_player;



};

