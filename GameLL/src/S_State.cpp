#include "S_State.h"
#include "S_Movement.h"
#include "S_Control.h"

S_State::S_State(SystemManager* i_systemMgr) : S_Base(System::State,i_systemMgr) {
	Bitmask req;
	req.turnOnBit((unsigned int)Component::State);
	m_requiredComponents.push_back(req);
	req.Clear();
	m_systemMgr->GetMessageHandler()->Subscribe(EntityMessage::Move, this);
	m_systemMgr->GetMessageHandler()->Subscribe(EntityMessage::Switch_State, this);
}
void S_State::Update(float i_dT) {
	EntityManagerNew* entities = m_systemMgr->GetEntityManager();
	for (auto& entity : m_entities) {
		C_State* state = entities->GetComponent<C_State>(entity, Component::State);
		if (state == nullptr) { return; }
		if (state->GetState() == EntityState::Walking) {
			if (state->GetDelay() > 0.f) {
				state->GetDelay() -= i_dT;
				continue;
			}
			Message msg((MessageType)EntityMessage::IsMoving);
			msg.m_receiver = entity;
			m_systemMgr->GetMessageHandler()->Dispatch(msg);
		}
	}
  }
void S_State::HandleEvent(const EntityId& i_entity, const EntityEvent& i_event) {
	switch (i_event)
	{
	case EntityEvent::Became_Idle:
		ChangeState(i_entity, EntityState::Idle, false);
		break;
	default:
		break;
	}
}
void S_State::Notify(const Message& i_msg) {
	if (!HasEntity(i_msg.m_receiver)) { return; }
	EntityMessage m = (EntityMessage)i_msg.m_type;
	switch (m) {
	case EntityMessage::Move:
	{
		C_State* state = m_systemMgr->GetEntityManager()->GetComponent<C_State>(i_msg.m_receiver, Component::State);
		if (state->GetState() == EntityState::Dying) { return; }
		EntityEvent e;
		if (i_msg.m_int == (int)Direction::Up) {
			e = EntityEvent::Moving_Up;
		}
		else if (i_msg.m_int == (int)Direction::Down) {
			e = EntityEvent::Moving_Down;
		}
		else if (i_msg.m_int == (int)Direction::Left) {
			e = EntityEvent::Moving_Left;
		}
		else if (i_msg.m_int == (int)Direction::Right) {
			e = EntityEvent::Moving_Right;
		}
		m_systemMgr->AddEvent(i_msg.m_receiver, (EventId)e);
		ChangeState(i_msg.m_receiver, EntityState::Walking, false);
		m_systemMgr->GetEntityManager()->GetComponent<C_State>(i_msg.m_receiver, Component::State)->SetDelay(0.4f);
		break;
	}
		

	case EntityMessage::Switch_State:
		ChangeState(i_msg.m_receiver, (EntityState)i_msg.m_int, false);
		break;
	}

}

void S_State::LoadMaterial(const std::string& i_materialName)
{
}

void S_State::LoadMaterials()
{
}

void S_State::ChangeState(const EntityId& i_entity, const EntityState& i_state, const bool& i_force) {
	EntityManagerNew* entities = m_systemMgr->GetEntityManager();
	for (auto& entity : m_entities) {
		C_State* state = entities->GetComponent<C_State>(entity, Component::State);
		if (!i_force && state->GetState() == EntityState::Dying) {
			return;
		}
		Message msgDelayed((MessageType)EntityMessage::State_Changed);
		msgDelayed.m_oldState = (int)state->GetState();
		state->SetState(i_state);
		msgDelayed.m_receiver = i_entity;
		msgDelayed.m_int = (int)i_state;
		m_systemMgr->GetMessageHandler()->QueueMessage(msgDelayed);
	}
}