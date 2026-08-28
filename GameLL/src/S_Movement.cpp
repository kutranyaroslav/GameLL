#include "S_Movement.h"
#include "Map.h"
S_Movement::S_Movement(SystemManager* i_systemMgr) :S_Base(System::Movement, i_systemMgr)  {
	Bitmask req;
	req.turnOnBit((unsigned int)Component::Position);
	req.turnOnBit((unsigned int)Component::Movable);
	m_requiredComponents.push_back(req);
	req.Clear();
	m_systemMgr->GetMessageHandler()->Subscribe(EntityMessage::IsMoving, this);
	m_gameMap = nullptr;
}

void S_Movement::Update(float i_dT) {
	if (!m_gameMap) { return; }
	EntityManagerNew* entities = m_systemMgr->GetEntityManager();
	for (auto& entity : m_entities) {
		C_Position* position = entities->GetComponent<C_Position>(entity, Component::Position);
		C_Movable* movable = entities->GetComponent<C_Movable>(entity, Component::Movable);
		MovementStep(i_dT,movable, position);
 		position->MoveBy(sf::Vector2f((movable->GetVelocity().x* i_dT),(movable->GetVelocity().y * i_dT)));
	}
}

void S_Movement::MovementStep(float i_dT, C_Movable* i_movable, C_Position* i_pos) {
	sf::Vector2f f_coefficient = GetTileFriction(i_pos->getElevation(), floor(i_pos->GetPosition().x / Sheet::Tile_Size),
		floor(i_pos->GetPosition().y / Sheet::Tile_Size));
	sf::Vector2f friction(i_movable->GetSpeed().x * f_coefficient.x, i_movable->GetSpeed().y * f_coefficient.y);
	i_movable->AddVelocity(i_movable->GetAcceleration() * i_dT);
	i_movable->SetAcceleration(sf::Vector2f(0.f, 0.f));
	i_movable->ApplyFriction(friction * i_dT);
	float magnitude = sqrt(
		(i_movable->GetVelocity().x * i_movable->GetVelocity().x) + (i_movable->GetVelocity().y * i_movable->GetVelocity().y)
	);
	if (magnitude <= i_movable->GetMaxVelocity()) { return; }
	float max_V = i_movable->GetMaxVelocity();
	i_movable->SetVelocity(sf::Vector2f(
		(i_movable->GetVelocity().x / magnitude) * max_V,
		(i_movable->GetVelocity().y / magnitude) * max_V
	));
}

const sf::Vector2f& S_Movement::GetTileFriction(unsigned int i_elevation, unsigned int i_x, unsigned int i_y) {
	Tile* t = nullptr;
	int elevation = static_cast<int>(i_elevation);
	while (!t && elevation >= 0) {
		t = m_gameMap->GetTile(i_x, i_y, i_elevation);
		--elevation;
	}
	return (t ? t->m_properties->m_friction : m_gameMap->GetDefaultTile()->m_friction);
}

void S_Movement::HandleEvent(const EntityId& i_entity, const EntityEvent& i_event) {
	switch (i_event)
	{
	case EntityEvent::Colliding_X:
		StopEntity(i_entity, Axis::x);
		break;
	case EntityEvent::Colliding_Y:
		StopEntity(i_entity, Axis::y);
		break;
	case EntityEvent::Moving_Left:
		SetDirection(i_entity, Direction::Left);
		break;
	case EntityEvent::Moving_Right:
		SetDirection(i_entity, Direction::Right);
		break;
	case EntityEvent::Moving_Up:
	{
		C_Movable* movable = m_systemMgr->GetEntityManager()->GetComponent<C_Movable>(i_entity, Component::Movable);
		if (movable->GetVelocity().x == 0) {
			SetDirection(i_entity, Direction::Up);
		}
	}
	break;
	case EntityEvent::Moving_Down:
	{
		C_Movable* movable = m_systemMgr->GetEntityManager()->GetComponent<C_Movable>(i_entity, Component::Movable);
		if (movable->GetVelocity().x == 0) {
			SetDirection(i_entity, Direction::Down);
		}
	}
	break;
	default:
		break;
	}
}
void S_Movement::Notify(const Message& i_message) {
	EntityManagerNew* eMgr = m_systemMgr->GetEntityManager();
	EntityMessage m = (EntityMessage)i_message.m_type;
	switch (m)

	{
	case EntityMessage::Move:
		break;
	case EntityMessage::IsMoving:
	{
		if (!HasEntity(i_message.m_receiver)) { return; }
		C_Movable* movable = eMgr->GetComponent<C_Movable>(i_message.m_receiver, Component::Movable);
		if (movable->GetVelocity() != sf::Vector2f(0.f, 0.f)) {
			return;
		}
		m_systemMgr->AddEvent(i_message.m_receiver, (EventId)EntityEvent::Became_Idle);
	}
		break;
	
	case EntityMessage::State_Changed:
		break;
	case EntityMessage::Direction_Changed:
		break;
	case EntityMessage::Switch_State:
		break;
	case EntityMessage::Attack_Action:
		break;
	case EntityMessage::Dead:
		break;
	default:
		break;
	}
}

void S_Movement::LoadMaterial(const std::string& i_materialName)
{
}

void S_Movement::LoadMaterials()
{
}

void S_Movement::StopEntity(const EntityId& i_entity, const Axis& i_axis) {
	C_Movable* movable = m_systemMgr->GetEntityManager()->GetComponent<C_Movable>(i_entity, Component::Movable);
	if (i_axis == Axis::x) {
		movable->SetVelocity(sf::Vector2f(0.f, movable->GetVelocity().y));
	}
	else if (i_axis == Axis::y) {
		movable->SetVelocity(sf::Vector2f(movable->GetVelocity().x, 0.f));
	}
}
void S_Movement::SetDirection(const EntityId& i_entity, const Direction& i_dir) {
	C_Movable* movable = m_systemMgr->GetEntityManager()->GetComponent<C_Movable>(i_entity, Component::Movable);
	C_State* state = m_systemMgr->GetEntityManager()->GetComponent<C_State>(i_entity, Component::State);
	Direction oldDir = movable->GetDirection();
	if (oldDir != i_dir) {
			movable->SetVelocity(sf::Vector2f(0.f, 0.f));
			state->SetState(EntityState::Changing_Axis);
	}
	movable->SetDirection(i_dir);
	Message msg((MessageType)EntityMessage::Direction_Changed);
	msg.m_receiver = i_entity;
	msg.m_int = (int)i_dir;
	m_systemMgr->GetMessageHandler()->Dispatch(msg);
}
void S_Movement::SetMap(Map* i_gameMap) { m_gameMap = i_gameMap; }

void S_Movement::SetWorld(World* i_world) {
	m_world = i_world;
}