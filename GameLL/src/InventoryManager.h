#pragma once
#include "Items.h"
#include "ItemManager.h"
#include <algorithm>
#include <vector>
#define SLOTS_AMOUNT_START 12

struct InventorySlot {
    Items::ItemId m_itemId = Items::INVALID_ITEM_ID;
    int m_amount  = 0;
    bool isEmpty() const {
        return itemId == Items::INVALID_ITEM_ID;
    }

};
class InventoryManager {
public:
    InventoryManager(ItemManager* i_itemManager);
    ~InventoryManager();
    bool AddItem(Items::ItemId i_itemId, int i_amount);
    bool RemoveItem(Items::ItemId i_itemId, int i_amount);
    bool MoveItem(std::size_t i_from, std::size_t i_to);

    std::size_t GetSlotCount() const;
private:
    ItemManager* m_itemManager = nullptr;
    std::vector<InventorySlot> m_inventorySlots;

};