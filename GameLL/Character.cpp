#include "Character.h"

Character::Character(EntityManager* i_entityManager) :
	EntityBase(i_entityManager),
	m_spriteSheet(m_entityManager->GetContext()->m_textureManager, m_entityManager->GetContext()->m_stateManager),
m_jumpVelocity(250), m_hitpoints(5)
{
	m_name = "Character";
}
Character::~Character(){}

void Character::Move(const Direction& i_dir) {
	if (GetState() == EntityState::Dying) { return; }
	m_spriteSheet.SetSpriteDir(i_dir);
	if (i_dir == Direction::Left) { Accelerate(-1, 0); }
	else { Accelerate(1, 0); }
	if (GetState() == EntityState::Idle) { SetState(EntityState::Walking); }
}

void Character::Jump() {
	if (GetState() == EntityState::Jumping || GetState() == EntityState::Dying || GetState() == EntityState::Hurt) {
		return;
	}
	SetState(EntityState::Jumping);
	AddVelocity(0, -m_jumpVelocity);
}
void Character::Attack() {
	if (GetState() == EntityState::Jumping || GetState() == EntityState::Attacking || GetState() == EntityState::Dying ||
		GetState() == EntityState::Hurt) {
		return;
	}
	SetState(EntityState::Attacking);
}

void Character::GetHurt(const int& i_damage) {
	if (GetState() == EntityState::Hurt || GetState() == EntityState::Dying) {
		return;
	}
	m_hitpoints = (m_hitpoints - i_damage > 0 ? m_hitpoints - i_damage : 0);
	if (m_hitpoints) { SetState(EntityState::Hurt); }
	else { SetState(EntityState::Dying); }
}

void Character::Load(const std::string& i_path) {
	std::fstream file;
	file.open(Utils::GetWorkingDirectory() + i_path);
	if (file.is_open()) {
		std::string line;
		while (std::getline(file, line)) {
			std::stringstream keystream(line);
			std::string type;
			keystream >> type;
			if (type == "Name") {
				keystream >> m_name;
			}
			else if (type == "SpriteSheet") {
				std::string path;
				keystream >> path;
				m_spriteSheet.LoadSheet(path);
			}
			else if (type == "Hitpoins") {
				keystream >> m_hitpoints;
			}
			else if (type == "BoundingBox") {
				sf::Vector2f boundingSize;
				keystream >> boundingSize.x >> boundingSize.y;
				SetSize(boundingSize.x, boundingSize.y);
			}
			else if (type == "DamageBox") {
				keystream >> m_attackAABBoffset.x >> m_attackAABBoffset.y >> m_attackAABB.width >> m_attackAABB.height;
			}
			else if (type == "Speed") {
				keystream >> m_speed.x >> m_speed.y;
			}
			else if (type == "JumpVelocity") {
				keystream >> m_jumpVelocity;
			}
			else if (type == "MaxVelocity") {
				keystream >> m_velocityMax.x >> m_velocityMax.y;
			}
			else {
				m_entityManager->GetContext()->m_textbox->Add("Character::Load can not read the type");
			}
		}
		file.close();
	}
}

void Character::UpdateAttackAABB() {
	m_attackAABB.left = (m_spriteSheet.GetSpriteDir() == Direction::Left ? (m_AABB.left - m_attackAABB.width) - m_attackAABBoffset.x :
		(m_AABB.left + m_attackAABB.width) + m_attackAABBoffset.x);
	m_attackAABB.top = m_AABB.top + m_attackAABBoffset.y;		
}

void Character::Animate() {
	EntityState state = GetState();
	if (state == EntityState::Walking && m_spriteSheet.GetCurrentAnim()->GetName() != "Walk") {
		m_spriteSheet.SetAnimation("Walk", true, true);
	}
	else if (state == EntityState::Jumping && m_spriteSheet.GetCurrentAnim()->GetName() != "Jump") {
		m_spriteSheet.SetAnimation("Jump", true, false);
	}
	else if (state == EntityState::Attacking && m_spriteSheet.GetCurrentAnim()->GetName() != "Attack") {
		m_spriteSheet.SetAnimation("Attack", true, false);
	}
	else if (state == EntityState::Hurt && m_spriteSheet.GetCurrentAnim()->GetName() != "Hurt") {
		m_spriteSheet.SetAnimation("Hurt", true, false);
	}
	else if (state == EntityState::Dying && m_spriteSheet.GetCurrentAnim()->GetName() != "Dying") {
		m_spriteSheet.SetAnimation("Dying", true, false);
	}
	else if (state == EntityState::Idle && m_spriteSheet.GetCurrentAnim()->GetName() != "Idle") {
		m_spriteSheet.SetAnimation("Idle", true, true);
	}
}

void Character::Update(float i_dT) {
	EntityBase::Update(i_dT);
	if (m_attackAABB.width != 0 && m_attackAABB.height != 0) {
		UpdateAttackAABB();
	}
	if (GetState() != EntityState::Dying && GetState() != EntityState::Attacking && GetState() != EntityState::Hurt) {
		if (abs(m_velocity.y) >= 0.001f) {
			SetState(EntityState::Jumping);
		}
		else if (abs(m_velocity.x) >= 0.001f) {
			SetState(EntityState::Walking);
		}
		else {
			SetState(EntityState::Idle);
		}
	}
	else if (GetState() == EntityState::Attacking || GetState() == EntityState::Hurt) {
		if (!m_spriteSheet.GetCurrentAnim()->IsPlaying()) {
			SetState(EntityState::Idle);
		}
	}
	else if (GetState() == EntityState::Dying) {
		if (!m_spriteSheet.GetCurrentAnim()->IsPlaying()) {
			m_entityManager->Remove(m_id);
		}
	}
	Animate();
	sf::Time deltaTime = sf::seconds(i_dT);
	m_spriteSheet.Update(deltaTime);
	m_spriteSheet.SetSpritePos(m_position);

	
}

void Character::Draw(sf::RenderWindow* i_wind) {
	m_spriteSheet.Draw(i_wind);
}