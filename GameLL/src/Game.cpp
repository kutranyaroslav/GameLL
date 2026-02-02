#include "Game.h"
#include "S_Sound.h"
Game::Game() :
	m_window(),
	m_stateManager(&m_context),
	m_entityManager(&m_systemManager, &m_textureManager),
	m_guiManager(m_window.GetEventManager(), &m_context), m_soundManager(&m_audioManager)
{
	manualFrame = 0;
	m_systemManager.SetEntityManager(&m_entityManager);
	m_context.m_wind = &m_window;
	m_context.m_eventManager = m_window.GetEventManager();
	m_context.m_textbox = m_window.GetTextbox();
	m_context.m_textureManager = &m_textureManager;
	m_context.m_stateManager = &m_stateManager;
	m_context.m_entityManager = &m_entityManager;
	m_context.m_systemManager = &m_systemManager;
	m_context.m_guiManager = &m_guiManager;
	m_context.m_fontManager = &m_fontManager;
	m_context.m_soundManager = &m_soundManager;
	//TO DO Erase after developement done
	if (m_context.m_stateManager) {
		if (m_context.m_stateManager->HasState(StateType::Developement)) {
			ErrorLogManager* log = m_context.m_errorLogManager->GetInstance();
			log->createFile(Utils::GetWorkingDirectory() + "dev//devlog.txt");
			try {
				THROW_EXCEPTION(1, "test error");
			}
			catch (cException& e){
				log->GetLogBuffer() << "******* Error *****\n";
				log->Flush();
				log->LogException(e);
				log->GetLogBuffer() << "*********************\n";
				log->Flush();

			}
		}
	}
	m_systemManager.GetSystem<S_Sound>(System::Sound)->SetUp(&m_audioManager, &m_soundManager);
	m_stateManager.SwitchTo(StateType::MainMenu);
}
Game::~Game(){
	m_fontManager.ReleaseResource("Main");
}

void Game::Update() {
	m_context.m_guiManager->Update(m_elapsed.asSeconds());
	GUI_Event event;
	while (m_context, m_guiManager.PollEvent(event)) {
		m_window.GetEventManager()->HandleEvent(event);
	}
 	m_window.Update();
	m_stateManager.Update(m_elapsed);
	m_soundManager.Update(m_elapsed.asSeconds());
}
Window* Game::getWindow() {
	return &m_window;
}

void Game::Render() {
	m_window.BeginDraw();
	m_stateManager.Draw();
	sf::View currentView = m_window.GetRenderWindow()->getView();
	m_window.GetRenderWindow()->setView(m_window.GetRenderWindow()->getDefaultView());
	m_context.m_guiManager->Draw(m_window.GetRenderWindow());
	m_window.GetRenderWindow()->setView(currentView);
	m_window.EndDraw();
}
void Game::LateUpdate() {
	m_stateManager.ProcessRequests();
	RestartClock();
}
void Game::RestartClock() {
	m_elapsed = m_clock.restart();
}