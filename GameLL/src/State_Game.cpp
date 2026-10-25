#include "State_Game.h"
#include "S_Movement.h"
#include "S_Collision.h"
#include "C_SpriteSheet.h"
#include "C_State.h"
#include "C_Movable.h"
#include "GUI_Manager.h"
#include "World.h"
#include <cmath>
State_Game::State_Game(StateManager* i_stateManager) :
	BaseState(i_stateManager),m_spriteSheet(i_stateManager->GetSharedContext()->m_textureManager)
{}

void State_Game::onCreate() {
	m_state = StateType::Game;
	Textbox* textboxTest = m_stateManager->GetSharedContext()->m_textbox;
	EventManager* evMgr = m_stateManager->GetSharedContext()->m_eventManager;
	GUI_Manager* gui = m_stateManager->GetSharedContext()->m_guiManager; 
	gui->LoadInterface(StateType::MainMenu, "MainMenu.interface", "MainMenu");
	m_stateManager->GetSharedContext()->m_wind->GetRenderWindow()->setView(
	*m_stateManager->GetSharedContext()->m_wind->GetGameView());
	m_view = m_stateManager->GetSharedContext()->m_wind->GetGameView();
	sf::Vector2u windowSize = m_stateManager->GetSharedContext()->m_wind->GetRenderWindow()->getSize();
	evMgr->AddCallback(StateType::Game, "Key_Escape", &State_Game::MainMenu, this);
	evMgr->AddCallback(StateType::Game, "Key_P", &State_Game::Pause, this);     
	Binding* terminalBinding = new Binding("OpenFieldTerminal");
	terminalBinding->BindEvent(EventType::KeyDown, EventInfo(sf::Keyboard::I));
	if (evMgr->AddBinding(terminalBinding)) {
		evMgr->AddCallback(StateType::Game, "OpenFieldTerminal", &State_Game::OpenFieldTerminal, this);
		m_dynamicCallbacks.push_back("OpenFieldTerminal");
	} else { delete terminalBinding; }

	evMgr->AddCallback(StateType::Game, "Player_Moveleft", &State_Game::Move, this);
	evMgr->AddCallback(StateType::Game, "Player_Moveright", &State_Game::Move, this);
	evMgr->AddCallback(StateType::Game, "Player_Moveup", &State_Game::Move, this);
	evMgr->AddCallback(StateType::Game, "Player_Movedown", &State_Game::Move, this);

	evMgr->AddCallback(StateType::Game, "Player_StopMoveleft", &State_Game::Stop, this);
	evMgr->AddCallback(StateType::Game, "Player_StopMoveright", &State_Game::Stop, this);
	evMgr->AddCallback(StateType::Game, "Player_StopMoveup", &State_Game::Stop, this);
	evMgr->AddCallback(StateType::Game, "Player_StopMovedown", &State_Game::Stop, this);
	// Map/tileset setup, m_player and the system wiring used to live here, but onCreate()
	// only ever runs once per state instance (StateManager keeps states alive and reuses
	// them - see Activate() for why that matters). Moved to Activate() so it re-runs every
	// time this state becomes active again, not just the first time it's created.

	Activate();
}

void State_Game::onDestroy() {
	EventManager* evMgr = m_stateManager->GetSharedContext()->m_eventManager;
	evMgr->RemoveCallback(StateType::Game, "Key_Escape");
	evMgr->RemoveCallback(StateType::Game, "Key_P");
	evMgr->RemoveCallback(StateType::Game, "Key_R");
	for (const std::string& callback : m_dynamicCallbacks) {
		evMgr->RemoveCallback(StateType::Game, callback);
		evMgr->RemoveBinding(callback);
	}
	m_dynamicCallbacks.clear();
}

void State_Game::Update(const sf::Time& i_time) {
	SharedContext* context = m_stateManager->GetSharedContext();
	UpdateCamera();
	UpdateMovementInput();
	context->m_systemManager->Update(i_time.asSeconds());

}

void State_Game::Draw() {
	SharedContext* ctx = m_stateManager->GetSharedContext();
	sf::RenderTexture* scene = ctx->m_wind->GetSceneTexture();
	scene->setView(*m_view);
	// Built once per frame, after the view is set, because the lights are
	// placed in world coordinates.
	ctx->m_systemManager->DrawLighting(ctx->m_wind);
	// Into the scene, so the bright pass can pick the particles up.
	ctx->m_systemManager->DrawParticles(ctx->m_wind);
	for (unsigned int i = 0; i < Sheet::Num_Layers; ++i) {
		ctx->m_world->Draw(*scene, scene->getView(), i); // Map::Draw уже берёт RenderTarget&, тут менять не надо
		ctx->m_systemManager->Draw(ctx->m_wind, i);
	}
	
}

void State_Game::Activate() {
	World* world = m_stateManager->GetSharedContext()->m_world;
	// если пришли из редактора (или из предыдущей игровой сессии) — продолжаем с той карты,
	// что там осталась активной; если это самый первый запуск без редактора вообще — дефолт MAP1
	std::string targetMap = world->GetCurrentMap() ? world->GetCurrentMap()->GetMapName() : "MAP1";
	if (!world->HasMap(targetMap)) {
		world->AddMap(targetMap);
	}
	world->AddConfiguredTilesets(targetMap);
	world->LoadMap(targetMap);
	if (!world->GetCurrentMap()) { m_player = -1; return; }

	m_player = world->GetCurrentMap()->GetPlayerId();
	m_stateManager->GetSharedContext()->m_systemManager->GetSystem<S_Movement>(System::Movement)->SetWorld(world);
	m_stateManager->GetSharedContext()->m_systemManager->GetSystem<S_Collision>(System::Collision)->SetWorld(world);
}
void State_Game::Deactivate() {
	m_moveLeft = m_moveRight = m_moveUp = m_moveDown = false;
	if (m_player == -1) { return; }
	C_Movable* movable = m_stateManager->GetSharedContext()->m_entityManager->GetComponent<C_Movable>(
		m_player, Component::Movable);
	if (movable) {
		movable->SetAcceleration(sf::Vector2f(0.f, 0.f));
		movable->SetVelocity(sf::Vector2f(0.f, 0.f));
	}
}

void State_Game::MainMenu(EventDetails* i_details) {
	m_stateManager->SwitchTo(StateType::MainMenu);
}

void State_Game::Pause(EventDetails* i_details) {
	m_stateManager->SwitchTo(StateType::Paused);
}

void State_Game::OpenFieldTerminal(EventDetails* i_details) {
	m_stateManager->SwitchTo(StateType::FieldTerminal);
}


void State_Game::UpdateCamera() {
	if (m_player == -1) { return; }
	if (!m_stateManager->GetSharedContext()->m_world->GetCurrentMap()) { return; }
	SharedContext* context = m_stateManager->GetSharedContext();
	C_Position* pos = m_stateManager->GetSharedContext()->m_entityManager->GetComponent<C_Position>
		(m_player, Component::Position);
	if (!pos) { return; } // m_player id is stale (e.g. purged by a map switch) - nothing to follow this frame
	m_view->setCenter(pos->GetPosition());
	sf::FloatRect viewSpace = context->m_wind->GetGameViewSpace();
	if (viewSpace.left <= 0) {
		m_view->setCenter(viewSpace.width / 2, m_view->getCenter().y);
		context->m_wind->GetRenderWindow()->setView(*m_view);
	}
	else if (viewSpace.left + viewSpace.width > (m_stateManager->GetSharedContext()
		->m_world->GetCurrentMap()->GetMapSize().x) * Sheet::Tile_Size) {
		m_view->setCenter(((m_stateManager->GetSharedContext()->m_world->GetCurrentMap()->GetMapSize().x) * Sheet::Tile_Size) 
			- (viewSpace.width / 2), m_view->getCenter().y);
		context->m_wind->GetRenderWindow()->setView(*m_view);
	}

	if (viewSpace.top <= 0) {
		m_view->setCenter(m_view->getCenter().x, viewSpace.height / 2);
		context->m_wind->GetRenderWindow()->setView(*m_view);
	}
	else if (viewSpace.top + viewSpace.height > (m_stateManager->GetSharedContext()->m_world->GetCurrentMap()->GetMapSize().y) 
		* Sheet::Tile_Size) {
		m_view->setCenter(m_view->getCenter().x, ((m_stateManager->GetSharedContext()->m_world->GetCurrentMap()->GetMapSize().y) 
			* Sheet::Tile_Size) - (viewSpace.height / 2));
		context->m_wind->GetRenderWindow()->setView(*m_view);
	}
}

void State_Game::Move(EventDetails* i_details){
	if (i_details->m_name == "Player_Moveleft") { m_moveLeft = true; }
	else if (i_details->m_name == "Player_Moveright") { m_moveRight = true; }
	else if (i_details->m_name == "Player_Moveup") { m_moveUp = true; }
	else if (i_details->m_name == "Player_Movedown") { m_moveDown = true; }

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
	if (!mov) { return; } // same stale-id guard as UpdateCamera()
	if (i_details->m_name == "Player_StopMoveleft" || i_details->m_name == "Player_StopMoveright") {
		if (i_details->m_name == "Player_StopMoveleft") { m_moveLeft = false; }
		else { m_moveRight = false; }
		if (!m_moveLeft && !m_moveRight) {
			mov->SetVelocity(sf::Vector2f(0.f, mov->GetVelocity().y));
		}
	}
	else {
		if (i_details->m_name == "Player_StopMoveup") { m_moveUp = false; }
		else { m_moveDown = false; }
		if (!m_moveUp && !m_moveDown) {
			mov->SetVelocity(sf::Vector2f(mov->GetVelocity().x, 0.f));
		}
	}
}
//Виталий Скапчевский
void State_Game::UpdateMovementInput() {
	if (m_player == -1) { return; }
	C_Movable* movable = m_stateManager->GetSharedContext()->m_entityManager->GetComponent<C_Movable>(
		m_player, Component::Movable);
	if (!movable) { return; }
	float x = (m_moveRight ? 1.f : 0.f) - (m_moveLeft ? 1.f : 0.f);
	float y = (m_moveDown ? 1.f : 0.f) - (m_moveUp ? 1.f : 0.f);
	const float length = std::sqrt(x * x + y * y);
	if (length > 1.f) { x /= length; y /= length; }
	const sf::Vector2f speed = movable->GetSpeed();
	movable->SetAcceleration(sf::Vector2f(x * speed.x, y * speed.y));
}
