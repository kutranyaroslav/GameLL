#pragma once
#include "C_Base.h"
enum class EntityState{Idle,Walking,Attacking,Hurt,Dying,Changing_Axis};
class C_State:public C_Base
{
public:
	C_State():C_Base(Component::State),m_delay(0.0f){}
	void ReadIn(std::stringstream& i_stream) {
		unsigned int state = 0;
		i_stream >> state;
		m_state = (EntityState)state;
	}
	EntityState GetState() { return m_state; }
	void SetState(EntityState i_state) {
		m_state = i_state;
	}
	float& GetDelay() { return m_delay; }
	void SetDelay(const float& i_delay) {
		m_delay = i_delay;
	}
private:
	EntityState m_state;
	float m_delay;
};

