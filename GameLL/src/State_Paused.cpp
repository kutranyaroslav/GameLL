#include "State_Paused.h"

State_Paused::State_Paused(StateManager* i_stateManager):
	BaseState(i_stateManager){}

void State_Paused::onCreate() {
	SetTransparent(true);
	m_font.loadFromFile("D:/Programming/SFML_5/SFML_5/ARIAL.TTF");
	
	m_text.setCharacterSize(30);
	m_text.setFont(m_font);
	m_text.setString(sf::String("PAUSED"));
	m_text.setStyle(sf::Text::Bold);
	sf::Vector2u windowSize = m_stateManager->GetSharedContext()->m_wind->GetWindowSize();
	sf::FloatRect textRect = m_text.getGlobalBounds();
	m_text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
	m_text.setPosition(windowSize.x / 2.0f, windowSize.y / 2.0f);
	m_rect.setSize(sf::Vector2f(windowSize));
	m_rect.setPosition(0, 0);
	m_rect.setFillColor(sf::Color(0, 0, 0, 150));
	EventManager* evMgr = m_stateManager->GetSharedContext()->m_eventManager;
	evMgr->AddCallback(StateType::Paused, "Key_P", &State_Paused::Unpause, this);

}

void State_Paused::onDestroy() {
	EventManager* evMgr = m_stateManager->GetSharedContext()->m_eventManager;
	evMgr->RemoveCallback(StateType::Paused, "Key_P");
}

void State_Paused::Draw() {
	sf::RenderWindow* wind = m_stateManager->GetSharedContext()->m_wind->GetRenderWindow();
	wind->draw(m_text);
	wind->draw(m_rect);
}

void State_Paused::Unpause(EventDetails* i_details) {
	m_stateManager->SwitchTo(StateType::Game);
}

void State_Paused::Activate(){}
void State_Paused::Deactivate(){}
void State_Paused::Update(const sf::Time& i_Time ){}