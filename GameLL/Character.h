#pragma once
#include "Entity.h"
#include "SpriteSheet.h"
#include "Directions.h"
class Character:public EntityBase
{
	friend class EntityManager;
public:
	Character(EntityManager* i_entityManager);
	virtual ~Character()= 0;
	void Move(const Direction& i_dir);
	void Jump();
	void Attack();
	void GetHurt(const int& i_damage);
	void Load(const std::string& i_path);
	virtual void OnEntityCollision(EntityBase* i_collider, bool i_attack) = 0;
	virtual void Update(float i_dT);
	virtual void Draw(sf::RenderWindow* i_wind);
protected:
	void UpdateAttackAABB();
	void Animate();
	
	SpriteSheet m_spriteSheet;
	float m_jumpVelocity;
	int m_hitpoints;
	sf::FloatRect m_attackAABB;
	sf::Vector2f m_attackAABBoffset;

};

