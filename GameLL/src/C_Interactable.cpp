#include "C_Interactable.h"

C_Interactable::C_Interactable() :
	C_Base(Component::Interactable), m_type(InteractionType::None),
	m_oneShot(false), m_used(false), m_amount(0.f),
	m_requiresKey(false), m_keyId(-1), m_quantity(0), m_targetId(-1)
{
}

// формат: "Component <componentId> <interactionType> <поля именно под этот тип>"
void C_Interactable::ReadIn(std::stringstream& i_stream) {
	unsigned int typeInt = 0;
	i_stream >> typeInt;
	m_type = (InteractionType)typeInt;

	// дефолты сбрасываются каждый раз - поле, которого нет в строке для этого типа,
	// никогда не унаследует мусор/значение от другого типа
	m_amount = 0.f;
	m_requiresKey = false;
	m_keyId = -1;
	m_itemId.clear();
	m_quantity = 0;
	m_textId.clear();
	m_targetId = -1;
	m_targetMap.clear();
	m_oneShot = false;

	unsigned int boolFlag = 0;
	switch (m_type) {
	case InteractionType::Door:
		i_stream >> boolFlag >> m_keyId;
		m_requiresKey = (boolFlag != 0);
		break;
	case InteractionType::AidKit:
		i_stream >> m_amount >> boolFlag;
		m_oneShot = (boolFlag != 0);
		break;
	case InteractionType::PickupItem:
		i_stream >> m_itemId >> m_quantity;
		m_oneShot = true;
		break;
	case InteractionType::ReadNote:
	case InteractionType::Examine:
	case InteractionType::Terminal:
		i_stream >> m_textId;
		break;
	case InteractionType::ToggleLever:
		i_stream >> m_targetId;
		break;
	case InteractionType::Elevator:
		i_stream >> m_targetMap;
		break;
	case InteractionType::SavePoint:
	case InteractionType::None:
	default:
		break; // доп. данные не нужны
	}
}