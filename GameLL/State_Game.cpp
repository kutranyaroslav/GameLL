#include "State_Game.h"

State_Game::State_Game(StateManager* i_stateManager) :
	BaseState(i_stateManager),m_spriteSheet(i_stateManager->GetSharedContext()->m_textureManager, i_stateManager)
{}

void State_Game::onCreate() {
	Textbox* textboxTest = m_stateManager->GetSharedContext()->m_textbox;
	EventManager* evMgr = m_stateManager->GetSharedContext()->m_eventManager;
	sf::Vector2u size = m_stateManager->GetSharedContext()->m_wind->GetWindowSize();
	m_view.setSize(size.x, size.y);
	m_view.setCenter(size.x / 2, size.y / 2);
	m_view.zoom(0.6f);
	m_stateManager->GetSharedContext()->m_wind->GetRenderWindow()->setView(m_view);

	sf::Vector2u windowSize = m_stateManager->GetSharedContext()->m_wind->GetRenderWindow()->getSize();
	evMgr->AddCallback(StateType::Game, "Key_Escape", &State_Game::MainMenu, this);
	evMgr->AddCallback(StateType::Game, "Key_P", &State_Game::Pause, this);
	evMgr->AddCallback(StateType::Game, "Key_R", &State_Game::Cast, this);

	//test integration of maps
	m_testMap = new Map(m_stateManager->GetSharedContext(), this);
	m_testMap->LoadMap("/Assets/maps/MAP1.map");
}

void State_Game::onDestroy() {
	EventManager* evMgr = m_stateManager->GetSharedContext()->m_eventManager;
	evMgr->RemoveCallback(StateType::Game, "Key_Escape");
	evMgr->RemoveCallback(StateType::Game, "Key_P");
	evMgr->RemoveCallback(StateType::Game, "Key_R");
	delete m_testMap;
	m_testMap = nullptr;
}

void State_Game::Update(const sf::Time& i_time) {
	SharedContext* context = m_stateManager->GetSharedContext();
 	EntityBase* player = context->m_entityManager->Find("Player");;
	Textbox* textbox = context->m_textbox;
	if (!player) {
		m_stateManager->GetSharedContext()->m_textbox->Add("player was not found");
		context->m_entityManager->Add(EntityType::Player, "Player");
		/*player->SetPosition(0,256);*/
	}
	else {
		m_view.setCenter(player->GetPosition());
		context->m_wind->GetRenderWindow()->setView(m_view);
	}
	sf::FloatRect viewSpace = context->m_wind->GetViewSpace();
	if(viewSpace.left<= 0 ){
		m_view.setCenter(viewSpace.width / 2, m_view.getCenter().y);
		context->m_wind->GetRenderWindow()->setView(m_view);
	}
	else if (viewSpace.left + viewSpace.width > (m_testMap->GetMapSize().x + 1) * Sheet::Tile_Size) {
		m_view.setCenter(((m_testMap->GetMapSize().x + 1) * Sheet::Tile_Size) - (viewSpace.width / 2), m_view.getCenter().y);
		context->m_wind->GetRenderWindow()->setView(m_view);
	}
	textbox->Add("player_pos.x - " + std::to_string(player->GetPosition().x) + " player_pos.y"
		+ std::to_string(player->GetPosition().y));
	/*textbox->Add("m_view left is " + std::to_string(m_view.getCenter().x -
		(m_view.getSize().x / 2)) + " m_view top is " +
		std::to_string(m_view.getCenter().y - (m_view.getSize().y / 2)) + " m_view width " +
		std::to_string(m_view.getSize().x) + " m_view height "  + std::to_string(m_view.getSize().y));*/
	m_testMap->Update(i_time.asSeconds());
	m_stateManager->GetSharedContext()->m_entityManager->Update(i_time.asSeconds());
	
}

void State_Game::Draw() {
	m_stateManager->GetSharedContext()->m_textbox
		->Render(*m_stateManager->GetSharedContext()->m_wind->GetRenderWindow());
	m_testMap->Draw();
	m_stateManager->GetSharedContext()->m_entityManager->Draw();
}

void State_Game::Activate(){}
void State_Game::Deactivate() {}

void State_Game::MainMenu(EventDetails* i_details) {
	m_stateManager->SwitchTo(StateType::MainMenu);
}

void State_Game::Pause(EventDetails* i_details) {
	m_stateManager->SwitchTo(StateType::Paused);
}

void State_Game::Cast(EventDetails* i_details) {
	if (m_spriteSheet.SetAnimation("Cast", true, true)) {
		Textbox* textbox = m_stateManager->GetSharedContext()->m_textbox;
		textbox->Add("Set Animation returned true");
	}
}