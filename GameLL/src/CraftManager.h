//
// Created by mariakutran on 19.09.2026.
//

#ifndef GAMELL_CRAFTMANAGER_H
#define GAMELL_CRAFTMANAGER_H
#pragma once
#include "ItemManager.h"
#include "Utilitites.h"
#include <unordered_map>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdint>

using RecepyId = std::int32_t;
struct CraftRecepy {
    std::string m_item1;
    std::string m_item2;
    std::string m_itemOutcome;
};


using RecepyContainer = std::unordered_map<RecepyId, CraftRecepy*>;

class CraftManager {
public:
    CraftManager(ItemManager* i_itemManager);
    ~CraftManager();
    void LoadRecepies(const std::string& i_path);
    int AddRecepy(const std::string& item1, const std::string& item2,
        const std::string& itemOutcome);
    void Purge();
    bool RemoveRecepy(const std::string& i_name);

    RecepyId GetRecepyId(const std::string &item1, const std::string &item2);

    RecepyId GetIdByStringId(const std::string& i_stringId) const;

private:
    RecepyContainer m_recepies;
    std::unordered_map<std::string, RecepyId> m_stringToId;
    ItemManager* m_itemManager;
    int m_idCounter = 0;

};


#endif //GAMELL_CRAFTMANAGER_H
