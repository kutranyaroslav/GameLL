#pragma once
#include "C_Base.h"


enum class InteractableType {
	None = 0,
	Door,
};



class C_Interactable : public C_Base
{
public:
	C_Interactable() :C_Base(Component::Interactable) {}
	void ReadIn(std::stringstream& i_stream) {
		StringToInteractableType(i_stream.str());
	}
	void StringToInteractableType(const std::string& i_type) {
		if (i_type == "Door") {
			m_type = InteractableType::Door;
		}
		else {
			m_type = InteractableType::None;
		}
	}
private:

	InteractableType m_type = InteractableType::None;

};
