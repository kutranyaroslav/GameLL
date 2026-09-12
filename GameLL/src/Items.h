#pragma once 
#include <cstdint>
#include <string>
using ItemId = std::uint32_t;

constexpr ItemId INVALID_ITEM_ID = 0;

enum ItemCategory {
	Consumable,
	Ammo, 
	Weapon,
	KeyItem,
};

struct ItemDef {

	ItemId  i_id = INVALID_ITEM_ID;
	std::string m_stringID = "";//.entity key 
	std::string m_name = "";
	std::string m_icon = ""; // icon which appears
	int m_maxStack = 0 ;
	ItemCategory m_category; 
	bool m_keyItem = false ;
	int m_useAmount = 0;
};