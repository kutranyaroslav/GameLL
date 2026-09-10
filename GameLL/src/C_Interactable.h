#pragma once
#include "C_Base.h"
#include <string>

enum class InteractionType {
	None = 0, Door, AidKit, PickupItem, ReadNote,
	ToggleLever, Examine, SavePoint, Terminal, Elevator
};

class C_Interactable : public C_Base
{
public:
	C_Interactable();
	void ReadIn(std::stringstream& i_stream) override;

	InteractionType GetType() const { return m_type; }
	bool IsOneShot() const { return m_oneShot; }
	bool IsUsed() const { return m_used; }
	void SetUsed(bool i_used) { m_used = i_used; }

	// AidKit
	float GetAmount() const { return m_amount; }
	// Door
	bool RequiresKey() const { return m_requiresKey; }
	int GetKeyId() const { return m_keyId; }
	// PickupItem
	const std::string& GetItemId() const { return m_itemId; }
	int GetQuantity() const { return m_quantity; }
	// ReadNote / Examine / Terminal
	const std::string& GetTextId() const { return m_textId; }
	// ToggleLever
	int GetTargetId() const { return m_targetId; }
	// Elevator
	const std::string& GetTargetMap() const { return m_targetMap; }

private:
	InteractionType m_type;
	bool m_oneShot;
	bool m_used;

	float m_amount;
	bool m_requiresKey;
	int m_keyId;
	std::string m_itemId;
	int m_quantity;
	std::string m_textId;
	int m_targetId;
	std::string m_targetMap;
};