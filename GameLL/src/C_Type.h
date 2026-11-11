//
// Created by mariakutran on 11.11.2026.
//

#ifndef GAMELL_C_TYPE_H
#define GAMELL_C_TYPE_H
#include "C_Base.h"

enum class EntityType{Player = 0, NPC, Item, Door};

class C_Type: public C_Base {
public:
    C_Type();
    void ReadIn(std::stringstream& i_stream) override;
    EntityType& GetType();
    void SetType(const EntityType& i_type);
private:
    EntityType m_type;
};


#endif //GAMELL_C_TYPE_H
