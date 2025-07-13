#include "S_Control.h"
S_Control::S_Control(SystemManager* i_systemMgr) : S_Base(System::Control,i_systemMgr) {
	Bitmask req;
	req.turnOnBit((unsigned int)Component::Controller);
	req.turnOnBit((unsigned int)Component::Movable);
	req.turnOnBit((unsigned int)Component::Position);
	m_requiredComponents.push_back(req);
	req.Clear();
}

void S_Control::HandleEvent(const EntityId& i_entity, const EntityEvent& i_event) {
	switch (i_event)
	{
	case EntityEvent::Moving_Left:
		MoveEntity(i_entity, Direction::Left);break;
	case EntityEvent::Moving_Right:
		MoveEntity(i_entity, Direction::Right);break;
	case EntityEvent::Moving_Up:
		MoveEntity(i_entity, Direction::Up);break;
	case EntityEvent::Moving_Down:
		MoveEntity(i_entity, Direction::Down);break;
	default:
		break;
	}
}

void S_Control::MoveEntity(const EntityId& i_entity, const Direction& i_dir) {
	C_Movable* mov = m_systemMgr->GetEntityManager()->GetComponent<C_Movable>(i_entity, Component::Movable);
	mov->Move(i_dir);
}
void S_Control::Update(float i_dT) {
	return;
}
void S_Control::Notify (const Message& i_message) {
	return;
}