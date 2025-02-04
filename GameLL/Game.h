#pragma once
#include "Window.h"
#include "StateManager.h"
#include "SpriteSheet.h"
#include "Entity.h"
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
	EntityManager m_entityManager;
	sf::Clock m_clock;
	sf::Time m_elapsed;
};

