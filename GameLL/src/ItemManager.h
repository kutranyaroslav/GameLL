#pragma once 
#include "Items.h"
#include <unordered_map>
class ItemManager
{
public:
    ItemManager();

    void LoadItems(const std::string& i_file = "Items.cfg");

    const Items::ItemDef* GetItemDef(Items::ItemId i_id) const;
    const Items::ItemDef* GetItemDef(const std::string& i_stringId) const;
    Items::ItemId GetIdByStringId(const std::string& i_stringId) const;

private:
    std::vector<Items::ItemDef> m_items;                        // m_items[i] holds the item with id == i + 1
    std::unordered_map<std::string, Items::ItemId> m_stringToId; // only needed at load time / from .entity files
};