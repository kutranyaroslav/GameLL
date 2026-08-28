#include "S_SheetAnimation.h"
#include "C_Movable.h"
S_SheetAnimation::S_SheetAnimation(SystemManager* i_systemMgr) : S_Base(System::SheetAnimation, i_systemMgr) {
	Bitmask req; 
	req.turnOnBit((unsigned int)Component::SpriteSheet);
	req.turnOnBit((unsigned int)Component::State);
	m_requiredComponents.push_back(req);
	req.Clear();
	m_systemMgr->GetMessageHandler()->Subscribe(EntityMessage::State_Changed, this);
}
void S_SheetAnimation::Update(float i_dT) {
	EntityManagerNew* entities = m_systemMgr->GetEntityManager();
	for (auto& entity : m_entities) {
		C_SpriteSheet* sprite = entities->GetComponent<C_SpriteSheet>(entity, Component::SpriteSheet);
		C_State* state = entities->GetComponent<C_State>(entity, Component::State);
		sprite->GetSpriteSheet()->Update(sf::seconds(i_dT));
		const std::string& animName = sprite->GetSpriteSheet()->GetCurrentAnim()->GetName();
		if (sprite->GetSpriteSheet()->GetCurrentAnim()->CheckMoved()) {
			int frame = sprite->GetSpriteSheet()->GetCurrentAnim()->GetCurrentFrame();
			Message msg((MessageType)EntityMessage::Frame_Change);
			msg.m_receiver = entity;
			msg.m_int = frame;
			m_systemMgr->GetMessageHandler()->Dispatch(msg);
		}
		if (animName == "Attack") {
			if (!sprite->GetSpriteSheet()->GetCurrentAnim()->IsPlaying()) {
				Message msg((MessageType)EntityMessage::Switch_State);
				msg.m_receiver = entity;
				msg.m_int = (int)EntityState::Idle;
				m_systemMgr->GetMessageHandler()->Dispatch(msg);
			}
			else if (sprite->GetSpriteSheet()->GetCurrentAnim()->IsInAction()) {
				Message msg((MessageType)EntityMessage::Attack_Action);
				msg.m_receiver = entity;
				m_systemMgr->GetMessageHandler()->Dispatch(msg);
			}
		}
		else if (animName == "Death" && sprite->GetSpriteSheet()->GetCurrentAnim()->IsPlaying()) {
			Message msg((MessageType)EntityMessage::Dead);
			msg.m_receiver = entity;
			m_systemMgr->GetMessageHandler()->Dispatch(msg);
		}
	}
}

void S_SheetAnimation::Notify(const Message& i_message) {
	if (!HasEntity(i_message.m_receiver)) { return; }
	EntityMessage m = (EntityMessage)i_message.m_type;
	C_SpriteSheet* sprite = m_systemMgr->GetEntityManager()->GetComponent<C_SpriteSheet>(i_message.m_receiver, Component::SpriteSheet);
	C_Movable* mov = m_systemMgr->GetEntityManager()->GetComponent<C_Movable>(i_message.m_receiver, Component::Movable);
	switch (m) {
	case EntityMessage::State_Changed: {
		EntityState s = (EntityState)i_message.m_int;
		switch (s) {
		case EntityState::Idle:
			ChangeAnimation(i_message.m_receiver, "Idle", true, true);
			break;
		case EntityState::Walking:
			ChangeAnimation(i_message.m_receiver, "Walk", true, true);
			break;
		case EntityState::Attacking:
			ChangeAnimation(i_message.m_receiver, "Attack", true, false);
			break;
		case EntityState::Hurt:
			break;
		case EntityState::Dying:
			ChangeAnimation(i_message.m_receiver, "Death", true, false);
			break;
		}
		break;
	}
		
	}
}
void S_SheetAnimation::ChangeAnimation(const EntityId& i_entity,const std::string& i_anim, bool i_play, bool i_loop) {
	C_SpriteSheet* sheet = m_systemMgr->GetEntityManager()->GetComponent<C_SpriteSheet>(i_entity, Component::SpriteSheet);
	sheet->GetSpriteSheet()->SetAnimation(i_anim, i_play, i_loop);
}

void S_SheetAnimation::HandleEvent(const EntityId& i_entity, const EntityEvent& i_event) {
	return; 
}

void S_SheetAnimation::LoadMaterial(const std::string& i_materialName)
{
}

void S_SheetAnimation::LoadMaterials()
{
}
