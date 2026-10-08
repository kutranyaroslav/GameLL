//
// Created by mariakutran on 05.10.2026.
//

#include "C_Inventory.h"

bool C_Inventory::AddItem(Items::ItemId i_itemId, int i_amount) {
    if (m_inventoryManager) {
        m_inventoryManager->AddItem(i_itemId, i_amount);
    }
}

bool C_Inventory::RemoveItem(Items::ItemId i_itemId, int i_amount) {
    if (m_inventoryManager) {
        m_inventoryManager->RemoveItem(i_itemId, i_amount);
    }
}

bool C_Inventory::MoveItem(std::size_t i_from, std::size_t i_to) {
    if (m_inventoryManager) {
        m_inventoryManager->MoveItem(i_from, i_to);
    }
}

void C_Inventory::SetUpInventorySize(std::size_t i_size) {
    if (m_inventoryManager) {
        m_inventoryManager->SetUpInventorySize(i_size);
    }
}

std::size_t C_Inventory::GetSlotCount() const {
    if (m_inventoryManager) {
        return m_inventoryManager->GetSlotCount();
    }
}
