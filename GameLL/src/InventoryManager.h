#pragma once

#include "Items.h"
#include <vector>
#ifdef min
#pragma message("================================")
#pragma message("MIN IS DEFINED HERE")
#pragma message("================================")
#endif
#define SLOTS_AMOUNT_START 12
inline constexpr std::size_t INVENTORY_GRID_COLUMNS = 4;

class ItemManager;
class CraftManager;

struct InventorySlot
{
    Items::ItemId m_itemId = Items::INVALID_ITEM_ID;
    int m_amount = 0;
    bool m_isContinuation = false;
    std::size_t m_parentIndex = 0;

    bool isEmpty() const
    {
        return m_itemId == Items::INVALID_ITEM_ID && !m_isContinuation;
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
    bool RemoveFromSlot(std::size_t i_slot, int i_amount);
    bool MoveItem(std::size_t i_from, std::size_t i_to);
    bool Craft(Items::ItemId i_first, Items::ItemId i_second);
    bool Rearrange();
    void SetUpInventorySize(std::size_t i_size);

    std::size_t GetSlotCount() const;
    const InventorySlot* GetSlot(std::size_t i_index) const;
    ItemManager* GetItemManager() const;
    Items::ItemId GetIdByStringId(const std::string& i_stringId) const;

    const std::string &GetIcon(const Items::ItemId &i_itemId);
private:
    std::size_t FindFreePlacement(unsigned int i_width) const;
    bool PlaceItem(std::size_t i_start, Items::ItemId i_itemId, int i_amount);
    void ClearItemAt(std::size_t i_index);
    ItemManager* m_itemManager = nullptr;
    CraftManager* m_craftManager = nullptr;
    std::vector<InventorySlot> m_inventorySlots;
};
