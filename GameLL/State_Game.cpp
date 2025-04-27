#include "State_Game.h"

State_Game::State_Game(StateManager* i_stateManager) :
	BaseState(i_stateManager),m_spriteSheet(i_stateManager->GetSharedContext()->m_textureManager)
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

	evMgr->AddCallback(StateType::Game, "Player_Moveleft", &State_Game::Move, this);
	evMgr->AddCallback(StateType::Game, "Player_Moveright", &State_Game::Move, this);
	evMgr->AddCallback(StateType::Game, "Player_Moveup", &State_Game::Move, this);
	evMgr->AddCallback(StateType::Game, "Player_Movedown", &State_Game::Move, this);

	//test integration of maps
	m_testMap = new Map(m_stateManager->GetSharedContext(), this);
	m_testMap->LoadMap("/Assets/maps/MAP1.map");

	m_player = m_testMap->GetPlayerId();
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
	UpdateCamera();
	m_testMap->Update(i_time.asSeconds());
	m_stateManager->GetSharedContext()->m_systemManager->Update(i_time.asSeconds());
	
}

void State_Game::Draw() {
	for (unsigned int i = 0; i < Sheet::Num_Layers; ++i) {
		m_testMap->Draw(i);
		m_stateManager->GetSharedContext()->m_systemManager->Draw(m_stateManager->GetSharedContext()->m_wind,i);
	}
	
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

void State_Game::UpdateCamera() {
	if (m_player == -1) { return; }
	SharedContext* context = m_stateManager->GetSharedContext();
	C_Position* pos = m_stateManager->GetSharedContext()->m_entityManager->GetComponent<C_Position>(m_player, Component::Position);
	m_view.setCenter(pos->GetPosition());
	context->m_wind->GetRenderWindow()->setView(m_view);
	sf::FloatRect viewSpace = context->m_wind->GetViewSpace();
	if (viewSpace.left <= 0) {
		m_view.setCenter(viewSpace.width / 2, m_view.getCenter().y);
		context->m_wind->GetRenderWindow()->setView(m_view);
	}
	else if (viewSpace.left + viewSpace.width > (m_testMap->GetMapSize().x) * Sheet::Tile_Size) {
		m_view.setCenter(((m_testMap->GetMapSize().x) * Sheet::Tile_Size) - (viewSpace.width / 2), m_view.getCenter().y);
		context->m_wind->GetRenderWindow()->setView(m_view);
	}

	if (viewSpace.top <= 0) {
		m_view.setCenter(m_view.getCenter().x, viewSpace.height / 2);
		context->m_wind->GetRenderWindow()->setView(m_view);
	}
	else if (viewSpace.top + viewSpace.height > (m_testMap->GetMapSize().y) * Sheet::Tile_Size) {
		m_view.setCenter(m_view.getCenter().x, ((m_testMap->GetMapSize().y) * Sheet::Tile_Size) - (viewSpace.height / 2));
		context->m_wind->GetRenderWindow()->setView(m_view);
	}
}

void State_Game::Move(EventDetails* i_details) {
	Message msg((MessageType)EntityMessage::Move);
	if (i_details->m_name == "Player_Moveleft") {
		msg.m_int = (int)Direction::Left;
	}
	else if (i_details->m_name == "Player_Moveright") {
		msg.m_int = (int)Direction::Right;
	}
	else if (i_details->m_name == "Player_Moveup") {
		msg.m_int = (int)Direction::Up;
	}
	else if (i_details->m_name == "Player_Movedown") {
		msg.m_int = (int)Direction::Down;
	}
	msg.m_receiver = m_player;
	m_stateManager->GetSharedContext()->m_systemManager->GetMessageHandler()->Dispatch(msg);
}