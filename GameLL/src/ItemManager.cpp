// ItemManager.cpp
#include "ItemManager.h"
#include "Utilitites.h"
#include <fstream>
#include <sstream>
#include <iostream>

ItemManager::ItemManager() {
    LoadItems();
}

// row format: id  "Name With Spaces"  icon  maxStack  category  keyItem(0/1)  useAmount
void ItemManager::LoadItems(const std::string& i_file) {
    std::string path = Utils::GetWorkingDirectory() + "nav//" + i_file;
    std::ifstream in(path);
    if (!in.is_open()) {
        std::cerr << "! ItemManager: can not open " << path << "\n";
        return;
    }

    std::string line;
    while (std::getline(in, line)) {
        if (line.empty() || line[0] == '|') { continue; } // comment/header row
        std::stringstream ss(line);

        Items::ItemDef def;
        std::string categoryStr;
        int keyItemFlag = 0;

        ss >> def.m_stringID;
        if (def.m_stringID.empty()) { continue; }
        Utils::ReadQuotedString(ss, def.m_name);
        ss >> def.m_icon >> def.m_maxStack >> categoryStr >> keyItemFlag >> def.m_useAmount;

        def.m_category = Items::ItemCategoryFromString(categoryStr);
        def.m_keyItem = (keyItemFlag != 0);
        def.m_id = static_cast<Items::ItemId>(m_items.size() + 1); // ids start at 1, 0 == INVALID_ITEM

        m_stringToId.emplace(def.m_stringID, def.m_id);
        m_items.push_back(def);
    }
}

const Items::ItemDef* ItemManager::GetItemDef(Items::ItemId i_id) const {
    if (i_id == Items::INVALID_ITEM_ID || i_id > m_items.size()) { return nullptr; }
    return &m_items[i_id - 1];
}

const Items::ItemDef* ItemManager::GetItemDef(const std::string& i_stringId) const {
    return GetItemDef(GetIdByStringId(i_stringId));
}

Items::ItemId ItemManager::GetIdByStringId(const std::string& i_stringId) const {
    auto itr = m_stringToId.find(i_stringId);
    return itr != m_stringToId.end() ? itr->second : Items::INVALID_ITEM_ID;
}