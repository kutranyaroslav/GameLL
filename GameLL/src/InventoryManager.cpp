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
    if (!itemDef || itemDef->m_maxStack <= 0 || itemDef->m_slotWidth == 0 ||
        itemDef->m_slotWidth > INVENTORY_GRID_COLUMNS) { return false; }
    const std::vector<InventorySlot> originalSlots = m_inventorySlots;
    // First try to fill existing stacks
    for (InventorySlot& slot: m_inventorySlots) {
        if (slot.m_isContinuation || slot.m_itemId != i_itemId) {
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
    while (i_amount > 0) {
        const std::size_t placement = FindFreePlacement(itemDef->m_slotWidth);
        if (placement == m_inventorySlots.size()) {
            m_inventorySlots = originalSlots;
            return false;
        }
        int amountToAdd = std::min(itemDef->m_maxStack, i_amount);
        if (!PlaceItem(placement, i_itemId, amountToAdd)) {
            m_inventorySlots = originalSlots;
            return false;
        }
        i_amount -= amountToAdd;
    }
    return true;
}

bool InventoryManager::RemoveItem(Items::ItemId i_itemId, int i_amount) {
    if (!m_itemManager || i_amount <= 0) {return false;}
    for (std::size_t index = 0; index < m_inventorySlots.size(); ++index) {
        InventorySlot& slot = m_inventorySlots[index];
        if (slot.m_isContinuation) { continue; }
        if (slot.m_itemId != i_itemId) { continue;}
        int m_amountToRemove = std::min(slot.m_amount, i_amount);
        slot.m_amount -= m_amountToRemove;
        i_amount -= m_amountToRemove;
        if (slot.m_amount == 0) {
            ClearItemAt(index);
        }
        if (i_amount == 0) { return true; }
    }
    return false;
}

bool InventoryManager::RemoveFromSlot(std::size_t i_slot, int i_amount) {
    if (i_slot >= m_inventorySlots.size() || i_amount <= 0) { return false; }
    if (m_inventorySlots[i_slot].m_isContinuation) {
        i_slot = m_inventorySlots[i_slot].m_parentIndex;
    }
    if (i_slot >= m_inventorySlots.size()) { return false; }
    InventorySlot& slot = m_inventorySlots[i_slot];
    if (slot.isEmpty() || i_amount > slot.m_amount) { return false; }
    slot.m_amount -= i_amount;
    if (slot.m_amount == 0) { ClearItemAt(i_slot); }
    return true;
}

bool InventoryManager::MoveItem(std::size_t i_from, std::size_t i_to) {
    if (i_from >= m_inventorySlots.size() || i_to >= m_inventorySlots.size()) {
        return false;
    }
    if (m_inventorySlots[i_from].m_isContinuation) {
        i_from = m_inventorySlots[i_from].m_parentIndex;
    }
    if (i_from >= m_inventorySlots.size() || m_inventorySlots[i_from].isEmpty()) { return false; }

    const InventorySlot moving = m_inventorySlots[i_from];
    const Items::ItemDef* def = m_itemManager ? m_itemManager->GetItemDef(moving.m_itemId) : nullptr;
    if (!def) { return false; }
    if (m_inventorySlots[i_to].m_isContinuation) { i_to = m_inventorySlots[i_to].m_parentIndex; }
    if (i_to % INVENTORY_GRID_COLUMNS + def->m_slotWidth > INVENTORY_GRID_COLUMNS) { return false; }

    const std::size_t oldEnd = i_from + def->m_slotWidth;
    const std::size_t newEnd = i_to + def->m_slotWidth;
    if (i_to == i_from) { return true; }
    for (std::size_t index = i_to; index < newEnd; ++index) {
        if (index >= m_inventorySlots.size()) { return false; }
        if (!m_inventorySlots[index].isEmpty() && (index < i_from || index >= oldEnd)) { return false; }
    }
    ClearItemAt(i_from);
    return PlaceItem(i_to, moving.m_itemId, moving.m_amount);
}

bool InventoryManager::Craft(Items::ItemId i_first, Items::ItemId i_second) {
    if (!m_itemManager || !m_craftManager) { return false; }
    const Items::ItemDef* first = m_itemManager->GetItemDef(i_first);
    const Items::ItemDef* second = m_itemManager->GetItemDef(i_second);
    if (!first || !second) { return false; }

    const std::string outcomeString = m_craftManager->GetRecepyOutcome(
        first->m_stringID, second->m_stringID);
    if (outcomeString.empty()) { return false; }
    const Items::ItemId outcome = m_itemManager->GetIdByStringId(outcomeString);
    if (outcome == Items::INVALID_ITEM_ID) { return false; }

    const std::vector<InventorySlot> originalSlots = m_inventorySlots;
    if (!RemoveItem(i_first, 1) || !RemoveItem(i_second, 1) || !AddItem(outcome, 1)) {
        m_inventorySlots = originalSlots;
        return false;
    }
    return true;
}

bool InventoryManager::Rearrange() {
    const std::vector<InventorySlot> originalSlots = m_inventorySlots;
    m_inventorySlots.assign(originalSlots.size(), InventorySlot{});
    for (std::size_t index = 0; index < originalSlots.size(); ++index) {
        const InventorySlot& slot = originalSlots[index];
        if (slot.m_isContinuation || slot.isEmpty()) { continue; }
        const Items::ItemDef* def = m_itemManager ? m_itemManager->GetItemDef(slot.m_itemId) : nullptr;
        if (!def) { m_inventorySlots = originalSlots; return false; }
        const std::size_t placement = FindFreePlacement(def->m_slotWidth);
        if (placement == m_inventorySlots.size() || !PlaceItem(placement, slot.m_itemId, slot.m_amount)) {
            m_inventorySlots = originalSlots;
            return false;
        }
    }
    return true;
}

std::size_t InventoryManager::FindFreePlacement(unsigned int i_width) const {
    if (i_width == 0 || i_width > INVENTORY_GRID_COLUMNS) { return m_inventorySlots.size(); }
    for (std::size_t start = 0; start < m_inventorySlots.size(); ++start) {
        if (start % INVENTORY_GRID_COLUMNS + i_width > INVENTORY_GRID_COLUMNS) { continue; }
        if (start + i_width > m_inventorySlots.size()) { continue; }
        bool free = true;
        for (std::size_t offset = 0; offset < i_width; ++offset) {
            if (!m_inventorySlots[start + offset].isEmpty()) { free = false; break; }
        }
        if (free) { return start; }
    }
    return m_inventorySlots.size();
}

bool InventoryManager::PlaceItem(std::size_t i_start, Items::ItemId i_itemId, int i_amount) {
    const Items::ItemDef* def = m_itemManager ? m_itemManager->GetItemDef(i_itemId) : nullptr;
    if (!def || i_start >= m_inventorySlots.size() ||
        i_start % INVENTORY_GRID_COLUMNS + def->m_slotWidth > INVENTORY_GRID_COLUMNS ||
        i_start + def->m_slotWidth > m_inventorySlots.size()) { return false; }
    for (std::size_t offset = 0; offset < def->m_slotWidth; ++offset) {
        if (!m_inventorySlots[i_start + offset].isEmpty()) { return false; }
    }
    m_inventorySlots[i_start] = InventorySlot{};
    m_inventorySlots[i_start].m_itemId = i_itemId;
    m_inventorySlots[i_start].m_amount = i_amount;
    for (std::size_t offset = 1; offset < def->m_slotWidth; ++offset) {
        InventorySlot& continuation = m_inventorySlots[i_start + offset];
        continuation = InventorySlot{};
        continuation.m_isContinuation = true;
        continuation.m_parentIndex = i_start;
    }
    return true;
}

void InventoryManager::ClearItemAt(std::size_t i_index) {
    if (i_index >= m_inventorySlots.size()) { return; }
    if (m_inventorySlots[i_index].m_isContinuation) {
        i_index = m_inventorySlots[i_index].m_parentIndex;
        if (i_index >= m_inventorySlots.size()) { return; }
    }
    const Items::ItemDef* def = m_itemManager
        ? m_itemManager->GetItemDef(m_inventorySlots[i_index].m_itemId) : nullptr;
    const unsigned int width = def ? def->m_slotWidth : 1;
    for (std::size_t offset = 0; offset < width && i_index + offset < m_inventorySlots.size(); ++offset) {
        m_inventorySlots[i_index + offset] = InventorySlot{};
    }
}

void InventoryManager::SetUpInventorySize(std::size_t i_size) {
    m_inventorySlots.resize(i_size);
}

std::size_t InventoryManager::GetSlotCount() const {
    return m_inventorySlots.size();
}

const InventorySlot* InventoryManager::GetSlot(std::size_t i_index) const {
    return i_index < m_inventorySlots.size() ? &m_inventorySlots[i_index] : nullptr;
}

ItemManager * InventoryManager::GetItemManager() const {
    return m_itemManager;
}

Items::ItemId InventoryManager::GetIdByStringId(const std::string &i_stringId) const {
    return m_itemManager? m_itemManager->GetIdByStringId(i_stringId) : Items::INVALID_ITEM_ID;
}

const std::string &InventoryManager::GetIcon(const Items::ItemId &i_itemId) {
    static const std::string empty;
    const Items::ItemDef* def = m_itemManager ? m_itemManager->GetItemDef(i_itemId) : nullptr;
    return def ? def->m_icon : empty;
}
