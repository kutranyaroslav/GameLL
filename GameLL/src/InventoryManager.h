#pragma once

#include "Items.h"
#include <vector>
#ifdef min
#pragma message("================================")
#pragma message("MIN IS DEFINED HERE")
#pragma message("================================")
#endif
#define SLOTS_AMOUNT_START 12

class ItemManager;
class CraftManager;

struct InventorySlot
{
    Items::ItemId m_itemId = Items::INVALID_ITEM_ID;
    int m_amount = 0;

    bool isEmpty() const
    {
        return m_itemId == Items::INVALID_ITEM_ID;
    }
};

class InventoryManager
{
public:
    InventoryManager(
        ItemManager* i_itemManager,
        CraftManager* i_craftManager
    );

    ~InventoryManager();

    bool AddItem(Items::ItemId i_itemId, int i_amount);
    bool RemoveItem(Items::ItemId i_itemId, int i_amount);
    bool MoveItem(std::size_t i_from, std::size_t i_to);
    void SetUpInventorySize(std::size_t i_size);

    std::size_t GetSlotCount() const;

private:
    ItemManager* m_itemManager = nullptr;
    CraftManager* m_craftManager = nullptr;
    std::vector<InventorySlot> m_inventorySlots;
};
