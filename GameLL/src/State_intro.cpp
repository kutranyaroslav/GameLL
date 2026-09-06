#include "State_intro.h"

namespace {
	// An id from textures.cfg, not a path.
	const char* INTRO_TEXTURE = "Bg1";
}


State_intro::State_intro(StateManager* i_stateManager):
	BaseState(i_stateManager)
{}
void State_intro::onCreate(){
	m_state = StateType::Intro;
	m_timePassed = 0.0f;
	sf::Vector2u windowSize = m_stateManager->GetSharedContext()->m_wind->GetRenderWindow()->getSize();
	// The splash is scene content, so it goes through the post processing chain
	// like the world does. It used to be loaded from an absolute path on one
	// developer machine, pointing at an Assets/Kunoichi folder that is not in
	// the repo, so the sprite was always empty.
	m_view = m_stateManager->GetSharedContext()->m_wind->GetUIView();
	TextureManager* textures = m_stateManager->GetSharedContext()->m_textureManager;
	if (textures && textures->RequireResource(INTRO_TEXTURE)) {
		m_introSprite.setTexture(*textures->GetResource(INTRO_TEXTURE));
		const sf::Vector2u size = textures->GetResource(INTRO_TEXTURE)->getSize();
		m_introSprite.setOrigin(size.x / 2.f, size.y / 2.f);
	}
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
	TextureManager* textures = m_stateManager->GetSharedContext()->m_textureManager;
	if (textures) { textures->ReleaseResource(INTRO_TEXTURE); }
}
void State_intro::Update(const sf::Time& i_time) {
	if (m_timePassed < 5.0f) {
		m_timePassed += i_time.asSeconds();
		m_introSprite.setPosition(m_introSprite.getPosition().x, m_introSprite.getPosition().y + (48 * i_time.asSeconds()));
	}
}

void State_intro::Draw() {
	// See State_Paused::Draw: the scene target is what reaches the window.
	sf::RenderTexture* scene = m_stateManager->GetSharedContext()->m_wind->GetSceneTexture();
	scene->setView(scene->getDefaultView());
	scene->draw(m_introSprite);
	if (m_timePassed > 5) {
		scene->draw(m_text);
	}
}
void State_intro::Continue(EventDetails* i_details) {
		m_stateManager->SwitchTo(StateType::MainMenu);
}
 void State_intro::Activate() {

}
void State_intro::Deactivate() {

}