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
    bool RemoveFromSlot(std::size_t i_slot, int i_amount);
    bool MoveItem(std::size_t i_from, std::size_t i_to);
    bool Craft(Items::ItemId i_first, Items::ItemId i_second);
    bool Use(std::size_t i_slot);
    bool Rearrange();
    void SetUpInventorySize(std::size_t i_size);

    std::size_t GetSlotCount() const;
    const InventorySlot* GetSlot(std::size_t i_index) const;
    Items::ItemId GetIdByStringId(const std::string& i_stringId) const;
    const std::string& GetIcon(const Items::ItemId i_itemId) const;
private:
    InventoryManager* m_inventoryManager = nullptr;

};


#endif //GAMELL_C_INVENTORY_H
