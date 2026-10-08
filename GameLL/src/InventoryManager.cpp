#include "InventoryManager.h"
#include "ItemManager.h"
#include "CraftManager.h"
#include <algorithm>
#undef min
InventoryManager::InventoryManager(
    ItemManager* i_itemManager,
    CraftManager* i_craftManager
)
    : m_itemManager(i_itemManager),
      m_craftManager(i_craftManager)
{
    m_inventorySlots.resize(SLOTS_AMOUNT_START);
}
InventoryManager::~InventoryManager() {}

bool InventoryManager::AddItem(Items::ItemId i_itemId, int i_amount) {
    if (!m_itemManager || i_amount  <= 0 ) {
        return false;
    }
    const Items::ItemDef* itemDef = m_itemManager->GetItemDef(i_itemId);
    if (!itemDef) { return false; }
    // First try to fill existing stacks
    for (InventorySlot& slot: m_inventorySlots) {
        if (slot.m_itemId != i_itemId) {
            continue;
        }
        if (slot.m_amount >= itemDef->m_maxStack) {
            continue;
        }
        int freeSpace = itemDef->m_maxStack - slot.m_amount;
        int AmountToAdd = std::min(freeSpace, i_amount);
        slot.m_amount += AmountToAdd;
        i_amount -= AmountToAdd;
        if (i_amount == 0) {
            return true;
        }
    }
    for (InventorySlot& slot: m_inventorySlots) {
        if (!slot.isEmpty()) {
            continue;
        }
        int amountToAdd = std::min(itemDef->m_maxStack, i_amount);
        slot.m_itemId = i_itemId;
        slot.m_amount =  amountToAdd;
        i_amount -= amountToAdd;
        if (i_amount == 0) {return true;}
    }
    return false;
}

bool InventoryManager::RemoveItem(Items::ItemId i_itemId, int i_amount) {
    if (!m_itemManager || i_amount <= 0) {return false;}
    for (InventorySlot& slot: m_inventorySlots) {
        if (slot.m_itemId != i_itemId) { continue;}
        int m_amountToRemove = std::min(slot.m_amount, i_amount);
        slot.m_amount -= m_amountToRemove;
        i_amount -= m_amountToRemove;
        if (slot.m_amount == 0) {
            slot.m_itemId = Items::INVALID_ITEM_ID;
        }
        if (i_amount == 0) { return true; }
    }
    return false;
}

bool InventoryManager::MoveItem(std::size_t i_from, std::size_t i_to) {
    if (i_from >= m_inventorySlots.size() || i_to >= m_inventorySlots.size()) {
        return false;
    }
    std::swap(m_inventorySlots[i_from], m_inventorySlots[i_to]);
    return true;
}

void InventoryManager::SetUpInventorySize(std::size_t i_size) {
    m_inventorySlots.resize(i_size);
}

std::size_t InventoryManager::GetSlotCount() const {
    return m_inventorySlots.size();
}
