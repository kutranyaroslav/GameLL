#pragma once
#include "Window.h"
#include "CraftManager.h"
#include "StateManager.h"
#include "SpriteSheet.h"
#include "SoundManager.h"
#include "GUI_Manager.h"
#include "ShaderManager.h"
#include "World.h"
#include "ItemManager.h"
#include "CraftManager.h"
class Game
{
public :
	Game();
	~Game();
	void Update();
	void Render();
	void LateUpdate();
	void RestartClock();
	Window* getWindow();

private:
	Window m_window;
	StateManager m_stateManager;
	TextureManager m_textureManager;
	SharedContext m_context;
	//system manager must be define before entity manaager to avoid problems with purgin in game engine
	SystemManager m_systemManager;
	EntityManagerNew m_entityManager;
	FontManager m_fontManager;
	GUI_Manager m_guiManager;
	AudioManager m_audioManager;
	SoundManager m_soundManager;
	ShaderManager m_shaderManager;
	ItemManager m_itemManager;
	CraftManager m_craftManager;
	World m_world;
	sf::Clock m_clock;
	sf::Time m_elapsed;
	unsigned int manualFrame;
};

