#include "State_intro.h"


State_intro::State_intro(StateManager* i_stateManager):
	BaseState(i_stateManager)
{}
void State_intro::onCreate(){
	m_timePassed = 0.0f;
	sf::Vector2u windowSize = m_stateManager->GetSharedContext()->m_wind->GetRenderWindow()->getSize();
	m_introTexture.loadFromFile("D:/Programming/SFML_5/SFML_5/assets/Kunoichi/Hurt.png");
	
	m_introSprite.setTexture(m_introTexture);
	m_introSprite.setOrigin(m_introTexture.getSize().x/2 , m_introTexture.getSize().y/2);
	m_introSprite.setPosition(windowSize.x / 2.0f, 0);
	
	m_font.loadFromFile( Utils::GetWorkingDirectory() + "Assets/Fonts/Arima-VariableFont_wght.ttf");
	
	m_text.setFont(m_font);
	m_text.setString({ "Press space to continue" });
	m_text.setCharacterSize(15);
	sf::FloatRect textRect = m_text.getGlobalBounds();
	m_text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
	m_text.setPosition(windowSize.x / 2, 0);
	
	EventManager* evMgr = m_stateManager->GetSharedContext()->m_eventManager;
	evMgr->AddCallback(StateType::Intro, "Intro_Continue", &State_intro::Continue, this);
	Textbox* TextBox = m_stateManager->GetSharedContext()->m_textbox;
	TextBox->Add("State intro is created");
}

void State_intro::onDestroy() {
	EventManager* evMgr = m_stateManager->GetSharedContext()->m_eventManager;
	evMgr->RemoveCallback(StateType::Intro, "Intro_Continue");
}
void State_intro::Update(const sf::Time& i_time) {
	if (m_timePassed < 5.0f) {
		m_timePassed += i_time.asSeconds();
		m_introSprite.setPosition(m_introSprite.getPosition().x, m_introSprite.getPosition().y + (48 * i_time.asSeconds()));
	}
}

void State_intro::Draw() {
	sf::RenderWindow* window = m_stateManager->GetSharedContext()->m_wind->GetRenderWindow();
	window->draw(m_introSprite);
	if (m_timePassed > 5) {
		window->draw(m_text);
	}
}
void State_intro::Continue(EventDetails* i_details) {
	if (m_timePassed > 5) {
		//this bullshit just for test purposes later on clean it up
		m_stateManager->Remove(StateType::Intro);
		m_stateManager->SwitchTo(StateType::MainMenu);
	}
}
 void State_intro::Activate() {

}
void State_intro::Deactivate() {

}