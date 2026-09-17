#pragma once 
#include <cstdint>
#include <string>
namespace Items {
	using ItemId = std::uint32_t;

	constexpr ItemId INVALID_ITEM_ID = 0;

	enum ItemCategory {
		Consumable,
		Ammo,
		Weapon,
		KeyItem,
	};

	std::string  ItemCategoryToString(const ItemCategory& i_category) {
		if (i_category == ItemCategory::Consumable) {
			return "Consumable";
		}
		else if (i_category == ItemCategory::Ammo) {
			return "Ammo";
		}
		else if (i_category == ItemCategory::Weapon) {
			return "Weapon";
		}
		else if (i_category == ItemCategory::KeyItem) {
			return "KeyItem";
		}
	}
	ItemCategory ItemCategoryFromString(const std::string& i_category) {
		if (i_category == "Consumable") {
			return ItemCategory::Consumable;
		}
		else if (i_category == "Ammo") {
			return ItemCategory::Ammo;
		}
		else if (i_category == "Weapon") {
			return ItemCategory::Weapon;
		}
		else if (i_category == "KeyItem") {
			return ItemCategory::KeyItem;
		}
	}
	struct ItemDef {

		ItemId  m_id = INVALID_ITEM_ID;
		std::string m_stringID = "";//.entity key 
		std::string m_name = "";
		std::string m_icon = ""; // icon which appears
		int m_maxStack = 0;
		ItemCategory m_category;
		bool m_keyItem = false;
		int m_useAmount = 0;
	};
}