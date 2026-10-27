//
// Created by mariakutran on 05.10.2026.
//

#include "C_Inventory.h"

bool C_Inventory::AddItem(Items::ItemId i_itemId, int i_amount) {
    if (m_inventoryManager) {
       return m_inventoryManager->AddItem(i_itemId, i_amount);

    }
    return false;
}

bool C_Inventory::RemoveItem(Items::ItemId i_itemId, int i_amount) {
    if (m_inventoryManager) {
       return m_inventoryManager->RemoveItem(i_itemId, i_amount);
    }
    return false;
}

bool C_Inventory::RemoveFromSlot(std::size_t i_slot, int i_amount) {
    return m_inventoryManager && m_inventoryManager->RemoveFromSlot(i_slot, i_amount);
}

bool C_Inventory::MoveItem(std::size_t i_from, std::size_t i_to) {
    if (m_inventoryManager) {
         return  m_inventoryManager->MoveItem(i_from, i_to);
    }
    return false;
}

bool C_Inventory::Craft(Items::ItemId i_first, Items::ItemId i_second) {
    return m_inventoryManager && m_inventoryManager->Craft(i_first, i_second);
}

bool C_Inventory::Use(std::size_t i_slot) {
    //the logic with checking for all the important data and parameters
    return RemoveItem(m_inventoryManager->GetSlot(i_slot)->m_itemId, 1 );
}

bool C_Inventory::Rearrange() {
    return m_inventoryManager && m_inventoryManager->Rearrange();
}

void C_Inventory::SetUpInventorySize(std::size_t i_size) {
    if (m_inventoryManager) {
        return m_inventoryManager->SetUpInventorySize(i_size);
    }
}

std::size_t C_Inventory::GetSlotCount() const {
    if (m_inventoryManager) {
        return m_inventoryManager->GetSlotCount();
    }
    return 0;
}

const InventorySlot* C_Inventory::GetSlot(std::size_t i_index) const {
    return m_inventoryManager ? m_inventoryManager->GetSlot(i_index) : nullptr;
}

Items::ItemId C_Inventory::GetIdByStringId(const std::string &i_stringId) const {
    return m_inventoryManager ?m_inventoryManager->GetIdByStringId(i_stringId) : Items::INVALID_ITEM_ID;
}

const std::string & C_Inventory::GetIcon(const Items::ItemId i_itemId) const {
    if (m_inventoryManager) {
        return m_inventoryManager->GetIcon(i_itemId);
    }else {return ""; }
}
