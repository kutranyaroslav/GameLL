#include "Game.h"
Game::Game():
	m_window(),
	m_stateManager(&m_context),
	m_entityManager(&m_context, 16)
{
	m_context.m_wind = &m_window;
	m_context.m_eventManager = m_window.GetEventManager();
	m_context.m_textbox = m_window.GetTextbox();
	m_context.m_textureManager = &m_textureManager;
	m_context.m_stateManager = &m_stateManager;
	m_context.m_entityManager = &m_entityManager;
	m_stateManager.SwitchTo(StateType::Intro);
	m_context.m_textbox->Add("You entered the game");
}
Game::~Game(){}

void Game::Update() {
	m_window.Update();
	m_stateManager.Update(m_elapsed);
}
Window* Game::getWindow() {
	return &m_window;
}

void Game::Render() {
	m_window.BeginDraw();
	m_stateManager.Draw();
	m_context.m_textbox->Render(*m_window.GetRenderWindow());
	m_window.EndDraw();
}
void Game::LateUpdate() {
	m_stateManager.ProcessRequests();
	RestartClock();
}
void Game::RestartClock() {
	m_elapsed += m_clock.restart();
}