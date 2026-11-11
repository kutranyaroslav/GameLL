//
// Created by mariakutran on 11.11.2026.
//

#include "C_Type.h"


C_Type::C_Type():C_Base(Component::Type) {}



void  C_Type::ReadIn(std::stringstream& i_stream) {
    int type; i_stream >> type;
    m_type = (EntityType)type;
}
EntityType& C_Type::GetType(){ return m_type;}
void C_Type::SetType(const EntityType& i_type) {
    m_type = i_type;
}