#include "Enemy.h"
//Enemy::Enemy(EntityManager* i_entityMgr) :
//	Character(i_entityMgr), m_hasDestination(false) {
//	m_type = EntityType::Enemy;
//}
//Enemy::~Enemy(){}
//
//void Enemy::OnEntityCollision(EntityBase* i_collider, bool i_attack) {
//	if (m_state == EntityState::Dying) { return; }
//	if (i_attack) { return; }
//	if (i_collider->GetType() != EntityType::Player) { return; }
//	Character* player = (Character*)i_collider;
//	SetState(EntityState::Attacking);
//	player->GetHurt(1);
//	if (m_position.x > player->GetPosition().x) {
//		player->AddVelocity(-m_speed.x, 0);
//		m_spriteSheet.SetSpriteDir(Direction::Left);
//	}
//	else {
//		player->AddVelocity(m_speed.x, 0);
//		m_spriteSheet.SetSpriteDir(Direction::Right);
//	}
//}
//
//void Enemy::Update(float i_dT) {
//	Character::Update(i_dT);
//	if (m_hasDestination) {
//		if (abs(m_destination.x - m_position.x) < 16) {
//			m_hasDestination = false;
//			return;
//		}
//		if (m_destination.x - m_position.x > 0) {
//			Move(Direction::Right);
//		}
//		else {
//			Move(Direction::Left);
//		}
//		if (m_collidingOnX) { m_hasDestination = false; }
//		return;
//	}
//	int random = rand() % 1000 + 1;
//	if (random != 1000) { return; }
//	int newX = rand() % 65 + 0;
//	if (rand() % 2) { newX = -newX; }
//	m_destination.x = m_position.x + newX;
//	if (m_destination.x < 0) { m_destination.x = 0; }
//	m_hasDestination = true;
//}