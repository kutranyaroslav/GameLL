#pragma once
#include "Character.h"
class Enemy :
    public Character
{
public:
    Enemy(EntityManager* i_entityMgr);
    ~Enemy() override;
    void OnEntityCollision(EntityBase* i_collider, bool i_attack) override;
    void Update(float i_dT) override;
private:
    sf::Vector2f m_destination;
    bool m_hasDestination;
};

