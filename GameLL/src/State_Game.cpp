#include "State_Game.h"
#include "S_Movement.h"
#include "S_Collision.h"
#include "C_SpriteSheet.h"
#include "C_State.h"
#include "C_Movable.h"
#include "GUI_Manager.h"
#include "World.h"
State_Game::State_Game(StateManager* i_stateManager) :
	BaseState(i_stateManager),m_spriteSheet(i_stateManager->GetSharedContext()->m_textureManager)
{}

void State_Game::onCreate() {
	m_state = StateType::Game;
	Textbox* textboxTest = m_stateManager->GetSharedContext()->m_textbox;
	EventManager* evMgr = m_stateManager->GetSharedContext()->m_eventManager;
	GUI_Manager* gui = m_stateManager->GetSharedContext()->m_guiManager; 
	gui->LoadInterface(StateType::MainMenu, "MainMenu.interface", "MainMenu");
	sf::Vector2u size = m_stateManager->GetSharedContext()->m_wind->GetWindowSize();
	m_view.setSize(size.x, size.y);
	m_view.setCenter(size.x / 2, size.y / 2);
	m_stateManager->GetSharedContext()->m_wind->GetRenderWindow()->setView(m_view);

	sf::Vector2u windowSize = m_stateManager->GetSharedContext()->m_wind->GetRenderWindow()->getSize();
	evMgr->AddCallback(StateType::Game, "Key_Escape", &State_Game::MainMenu, this);
	evMgr->AddCallback(StateType::Game, "Key_P", &State_Game::Pause, this);     

	evMgr->AddCallback(StateType::Game, "Player_Moveleft", &State_Game::Move, this);
	evMgr->AddCallback(StateType::Game, "Player_Moveright", &State_Game::Move, this);
	evMgr->AddCallback(StateType::Game, "Player_Moveup", &State_Game::Move, this);
	evMgr->AddCallback(StateType::Game, "Player_Movedown", &State_Game::Move, this);

	evMgr->AddCallback(StateType::Game, "Player_StopMoveleft", &State_Game::Stop, this);
	evMgr->AddCallback(StateType::Game, "Player_StopMoveright", &State_Game::Stop, this);
	evMgr->AddCallback(StateType::Game, "Player_StopMoveup", &State_Game::Stop, this);
	evMgr->AddCallback(StateType::Game, "Player_StopMovedown", &State_Game::Stop, this);
	//test integration of map
	m_stateManager->GetSharedContext()->m_world->AddMap("MAP1", "TestTileset", "Tiles.cfg", "Tilesheet");
	m_stateManager->GetSharedContext()->m_world->AddTileset("MAP1", "TestTileset2", "Tiles.cfg", "Tilesheet2");
	m_stateManager->GetSharedContext()->m_world->LoadMap("MAP1");
	if (!m_stateManager->GetSharedContext()->m_world->GetCurrentMap()) { return; }
	
	m_player = m_stateManager->GetSharedContext()->m_world->GetCurrentMap()->GetPlayerId();
	m_stateManager->GetSharedContext()->m_systemManager->GetSystem<S_Movement>(System::Movement)->SetWorld
		(m_stateManager->GetSharedContext()->m_world);
	m_stateManager->GetSharedContext()->m_systemManager->GetSystem<S_Collision>(System::Collision)->SetWorld(
	m_stateManager->GetSharedContext()->m_world);
	
}

void State_Game::onDestroy() {
	EventManager* evMgr = m_stateManager->GetSharedContext()->m_eventManager;
	evMgr->RemoveCallback(StateType::Game, "Key_Escape");
	evMgr->RemoveCallback(StateType::Game, "Key_P");
	evMgr->RemoveCallback(StateType::Game, "Key_R");
}

void State_Game::Update(const sf::Time& i_time) {
	SharedContext* context = m_stateManager->GetSharedContext();
	UpdateCamera();
	m_stateManager->GetSharedContext()->m_systemManager->Update(i_time.asSeconds());

}

void State_Game::Draw() {
	for (unsigned int i = 0; i < Sheet::Num_Layers; ++i) {
		m_stateManager->GetSharedContext()->m_world->Draw(*m_stateManager->GetSharedContext()->m_wind->GetRenderWindow(),
			m_stateManager->GetSharedContext()->m_wind->GetRenderWindow()->getView(), i);
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


void State_Game::UpdateCamera() {
	if (m_player == -1) { return; }
	if (!m_stateManager->GetSharedContext()->m_world->GetCurrentMap()) { return; }
	SharedContext* context = m_stateManager->GetSharedContext();
	C_Position* pos = m_stateManager->GetSharedContext()->m_entityManager->GetComponent<C_Position>
		(m_player, Component::Position);
	m_view.setCenter(pos->GetPosition());
	context->m_wind->GetRenderWindow()->setView(m_view);
	sf::FloatRect viewSpace = context->m_wind->GetViewSpace();
	if (viewSpace.left <= 0) {
		m_view.setCenter(viewSpace.width / 2, m_view.getCenter().y);
		context->m_wind->GetRenderWindow()->setView(m_view);
	}
	else if (viewSpace.left + viewSpace.width > (m_stateManager->GetSharedContext()
		->m_world->GetCurrentMap()->GetMapSize().x) * Sheet::Tile_Size) {
		m_view.setCenter(((m_stateManager->GetSharedContext()->m_world->GetCurrentMap()->GetMapSize().x) * Sheet::Tile_Size) 
			- (viewSpace.width / 2), m_view.getCenter().y);
		context->m_wind->GetRenderWindow()->setView(m_view);
	}

	if (viewSpace.top <= 0) {
		m_view.setCenter(m_view.getCenter().x, viewSpace.height / 2);
		context->m_wind->GetRenderWindow()->setView(m_view);
	}
	else if (viewSpace.top + viewSpace.height > (m_stateManager->GetSharedContext()->m_world->GetCurrentMap()->GetMapSize().y) 
		* Sheet::Tile_Size) {
		m_view.setCenter(m_view.getCenter().x, ((m_stateManager->GetSharedContext()->m_world->GetCurrentMap()->GetMapSize().y) 
			* Sheet::Tile_Size) - (viewSpace.height / 2));
		context->m_wind->GetRenderWindow()->setView(m_view);
	}
}

void State_Game::Move(EventDetails* i_details){
	
	Message msg2((MessageType)EntityMessage::Move);
	if (i_details->m_name == "Player_Moveleft") {
		msg2.m_int = (int)Direction::Left;
	}
	else if (i_details->m_name == "Player_Moveright") {
		msg2.m_int = (int)Direction::Right;
	}
	else if (i_details->m_name == "Player_Moveup") {
		msg2.m_int = (int)Direction::Up;
	}
	else if (i_details->m_name == "Player_Movedown") {
		msg2.m_int = (int)Direction::Down;
	}
	msg2.m_receiver = m_player;
	m_stateManager->GetSharedContext()->m_systemManager->GetMessageHandler()->Dispatch(msg2);
}
void State_Game::Stop(EventDetails* i_details) {
	C_Movable* mov = m_stateManager->GetSharedContext()->m_entityManager->GetComponent<C_Movable>(m_player, Component::Movable);
	if (i_details->m_name == "Player_StopMoveleft" || i_details->m_name == "Player_StopMoveright") {
		mov->SetVelocity(sf::Vector2f(0.f, mov->GetVelocity().y));
	}
	else {
		mov->SetVelocity(sf::Vector2f(mov->GetVelocity().x, 0.f));
	}
}