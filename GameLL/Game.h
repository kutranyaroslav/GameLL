#pragma once
#include "Window.h"
#include "StateManager.h"
#include "SpriteSheet.h"
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
	sf::Clock m_clock;
	sf::Time m_elapsed;
};

