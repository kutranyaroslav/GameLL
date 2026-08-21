#pragma once
#include "Window.h"
#include "StateManager.h"
#include "SpriteSheet.h"
#include "SoundManager.h"
#include "GUI_Manager.h"
#include "World.h"
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
	SystemManager m_systemManager;
	EntityManagerNew m_entityManager;
	FontManager m_fontManager;
	GUI_Manager m_guiManager;
	AudioManager m_audioManager;
	SoundManager m_soundManager;
	World m_world;
	sf::Clock m_clock;
	sf::Time m_elapsed;
	unsigned int manualFrame;
};

