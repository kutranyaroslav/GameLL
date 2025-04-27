 #include "Player.h"

//Player::Player(EntityManager* i_entityMgr) :
//	Character(i_entityMgr) {
//	Load("Player.char");
//	m_type = EntityType::Player; 
//	EventManager* eventMgr = m_entityManager->GetContext()->m_eventManager;
//	eventMgr->AddCallback<Player>(StateType::Game, "Player_Moveleft", &Player::React, this);
//	eventMgr->AddCallback<Player>(StateType::Game, "Player_Moveright", &Player::React, this);
//	eventMgr->AddCallback<Player>(StateType::Game, "Player_Jump", &Player::React, this);
//	eventMgr->AddCallback<Player>(StateType::Game, "Player_Attack", &Player::React, this);
//}
//
//Player::~Player() {
//	EventManager* eventMgr = m_entityManager->GetContext()->m_eventManager;
//	eventMgr->RemoveCallback(StateType::Game, "Player_Moveleft");
//	eventMgr->RemoveCallback(StateType::Game, "Player_Moveright");
//	eventMgr->RemoveCallback(StateType::Game, "Player_Jump");
//	eventMgr->RemoveCallback(StateType::Game, "Player_Attack");
//}
//
//void Player::OnEntityCollision(EntityBase* i_collider, bool i_attack) {
//	if (m_state == EntityState::Dying) { return; }
//	if (i_attack) {
//		if (m_state != EntityState::Attacking) { return; }
//		if (!m_spriteSheet.GetCurrentAnim()->IsInAction()) { return; }
//		if (i_collider->GetType() != EntityType::Enemy && i_collider->GetType() != EntityType::Player) {
//			return;
//		}
//		Character* opponent = (Character*)i_collider;
//		opponent->GetHurt(1);
//		if (m_position.x > opponent->GetPosition().x) {
//			opponent->AddVelocity(-32, 0);
//		}
//		else {
//			opponent->AddVelocity(32, 0);
//		}
//	}
//	else {
//		//to be expanded
//	}
//}
//
//void Player::React(EventDetails* i_details) {
//	if (i_details->m_name == "Player_Moveright") {
//		Move(Direction::Right);
//	}
//	else if (i_details->m_name == "Player_Moveleft") {
//		Move(Direction::Left);
//	}
//	else if (i_details->m_name == "Player_Jump") {
//		Jump();
//	}
//	else if (i_details->m_name == "Player_Attack") {
//		Attack();
//	}
//}