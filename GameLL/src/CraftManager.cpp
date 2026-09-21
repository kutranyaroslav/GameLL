//
// Created by mariakutran on 19.09.2026.
//

#include "CraftManager.h"
CraftManager::CraftManager(ItemManager* i_itemManager): m_itemManager(i_itemManager) {
    LoadRecepies("Recepies.cfg");
}
CraftManager::~CraftManager() {
    Purge();
}



void CraftManager::LoadRecepies(const std::string& i_path) {
    std::string path = Utils::GetWorkingDirectory()+ "nav//" + i_path;
    std::fstream file;
    file.open(path);
    if (!file.is_open()) {return;}
    std::string line;
    while (std::getline(file, line)) {
        if (line[0] == '|' || line.empty()){continue;}
        std::stringstream ss(line);
        std::string name;
        std::string item1;
        std::string item2;
        std::string itemOutcome;
        ss >> name >> item1 >> item2 >> itemOutcome;
        RecepyId id =  AddRecepy(item1, item2, itemOutcome);
        if (id != -1 ) {
            m_stringToId.emplace(name, id) ;
        }


    }


}

RecepyId  CraftManager::AddRecepy(const std::string &item1,
    const std::string &item2, const std::string& itemOutcome) {
    RecepyId id = -1;
    if (!m_itemManager){return id; }
    if (!m_itemManager->GetItemDef(item1) || !m_itemManager->GetItemDef(item2)
    || !m_itemManager->GetItemDef(itemOutcome)) { return id;}
    CraftRecepy* recepy = new CraftRecepy();
    recepy->m_item1 = item1;
    recepy->m_item2 = item2;
    recepy->m_itemOutcome =  itemOutcome;
    id = m_idCounter;
    if ( m_recepies.emplace(id, recepy).second == true) {
        m_idCounter++;
        return id;
    }else {
        return -1;
    }

}

bool CraftManager::RemoveRecepy(const std::string& i_name) {
    int id  = GetIdByStringId(i_name);
    auto itr = m_recepies.find(id);
    if (itr != m_recepies.end()) {
        delete itr->second;
        itr->second = nullptr;
        m_recepies.erase(itr);
        return true;
    }else {
        return false;
    }
}

RecepyId CraftManager::GetRecepyId(const std::string &item1, const std::string &item2) {
    if (m_itemManager) {
        if (m_itemManager->GetItemDef(item1) && m_itemManager->GetItemDef(item2)) {
            for (auto& recepy : m_recepies) {
                if (recepy.second->m_item1 == item1 && recepy.second->m_item2 == item2) {
                    return recepy.first;
                }
            }
        }
    }
    return -1;
}

RecepyId CraftManager::GetIdByStringId(const std::string &i_stringId) const {
    auto itr = m_stringToId.find(i_stringId);
    if (itr == m_stringToId.end()) { return -1;}
    return itr->second;
}


void CraftManager::Purge() {
    for (auto& itr: m_recepies) {
        delete itr.second ;
        itr.second = nullptr;
    }
    m_recepies.clear();
}
