#pragma once
#include "C_Base.h"
enum class EntityState{Idle,Walking,Attacking,Hurt,Dying};
class C_State:public C_Base
{
public:
	C_State():C_Base(Component::State){}
	void ReadIn(std::stringstream& i_stream) {
		unsigned int state = 0;
		i_stream >> state;
		m_state = (EntityState)state;
	}
	EntityState GetState() { return m_state; }
	void SetState(EntityState i_state) {
		m_state = i_state;
	}
private:
	EntityState m_state;
};

