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
		if (!sprite || !sprite->GetSpriteSheet()) { continue; }
		sprite->GetSpriteSheet()->Update(sf::seconds(i_dT));
		auto door = m_doorAnimationToFinish.find(entity);
		if (door != m_doorAnimationToFinish.end()) {
			Anim_Base* animation = sprite->GetSpriteSheet()->GetCurrentAnim();

			const std::string expectedAnimation =
				door->second ? "Door_Open" : "Door_Close";

			if (animation &&
				animation->GetName() == expectedAnimation &&
				!animation->IsPlaying()) {

				Message msg((MessageType)EntityMessage::Interaction_Completed);
				msg.m_receiver = entity;
				msg.m_int = door->second ? 1 : 0;

				m_systemMgr->GetMessageHandler()->Dispatch(msg);
				m_doorAnimationToFinish.erase(door);
			}
		}
		Anim_Base* currentAnimation = sprite->GetSpriteSheet()->GetCurrentAnim();
		if (!currentAnimation) { continue; }
		const std::string& animName = currentAnimation->GetName();
		if (currentAnimation->CheckMoved()) {
			int frame = currentAnimation->GetCurrentFrame();
			Message msg((MessageType)EntityMessage::Frame_Change);
			msg.m_receiver = entity;
			msg.m_int = frame;
			m_systemMgr->GetMessageHandler()->Dispatch(msg);
		}
		if (animName == "Attack") {
			if (!currentAnimation->IsPlaying()) {
				Message msg((MessageType)EntityMessage::Switch_State);
				msg.m_receiver = entity;
				msg.m_int = (int)EntityState::Idle;
				m_systemMgr->GetMessageHandler()->Dispatch(msg);
			}
			else if (currentAnimation->IsInAction()) {
				Message msg((MessageType)EntityMessage::Attack_Action);
				msg.m_receiver = entity;
				m_systemMgr->GetMessageHandler()->Dispatch(msg);
			}
		}
		else if (animName == "Death" && currentAnimation->IsPlaying()) {
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
bool S_SheetAnimation::ChangeAnimation(const EntityId& i_entity,const std::string& i_anim, bool i_play, bool i_loop) {
	EntityManagerNew* entities = m_systemMgr->GetEntityManager();
	if (!entities) { return false; }
	C_SpriteSheet* sheet = entities->GetComponent<C_SpriteSheet>(i_entity, Component::SpriteSheet);
	if (!sheet || !sheet->GetSpriteSheet()) { return false; }
	return sheet->GetSpriteSheet()->SetAnimation(i_anim, i_play, i_loop);
}

void S_SheetAnimation::HandleEvent(const EntityId& i_entity, const EntityEvent& i_event) {
	switch (i_event) {
		case EntityEvent::Door_Open:
			if (ChangeAnimation(i_entity, "Door_Open", true, false)) {
				m_doorAnimationToFinish[i_entity] = true;
			}
			break;

		case EntityEvent::Door_Close:
			if (ChangeAnimation(i_entity, "Door_Close", true, false)) {
				m_doorAnimationToFinish[i_entity] = false;
			}
			break;

		default:
			break;
	}
}

void S_SheetAnimation::LoadMaterial(const std::string& i_materialName)
{
}

void S_SheetAnimation::LoadMaterials()
{
}
