#pragma once
#include "Character.h"
class Player :
    public Character
{
public:
    Player(EntityManager* i_entityMgr);
    ~Player() override;

    void OnEntityCollision(EntityBase* i_collider, bool i_attack) override;
    void React(EventDetails* i_details);
};

