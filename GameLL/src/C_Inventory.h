//
// Created by mariakutran on 05.10.2026.
//
#include "C_Base.h"
#include "InventoryManager.h"
#ifndef GAMELL_C_INVENTORY_H
#define GAMELL_C_INVENTORY_H


class C_Inventory: public C_Base {
public:
    C_Inventory():C_Base(Component::Inventory){};
    virtual ~C_Inventory(){};
    void ReadIn(std::stringstream& i_stream)override{};
    void SetInventoryManager(InventoryManager* i_inventoryManager) {
        m_inventoryManager = i_inventoryManager;
    }
    bool AddItem(Items::ItemId i_itemId, int i_amount);
    bool RemoveItem(Items::ItemId i_itemId, int i_amount);
    bool MoveItem(std::size_t i_from, std::size_t i_to);
    void SetUpInventorySize(std::size_t i_size);

    std::size_t GetSlotCount() const;
private:
    InventoryManager* m_inventoryManager;

};


#endif //GAMELL_C_INVENTORY_H
