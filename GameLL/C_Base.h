#pragma once
#include "ECS_Types.h"
#include <sstream>
class C_Base
{
public: 
	C_Base(const Component& i_type) :m_type(i_type) {};
	virtual ~C_Base(); 
	Component getComponentType() { return m_type; }
	friend std::stringstream& operator>>(std::stringstream& i_stream, C_Base& b) {
		b.ReadIn(i_stream);
		return i_stream;
	}
	virtual void ReadIn(std::stringstream& i_stream) = 0;
protected:
	Component m_type;

};

