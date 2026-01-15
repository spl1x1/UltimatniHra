//
// Created by honzi on 31. 12. 2025.
//

#include "../../include/Items/inventory.h"

#include "../../include/Window/Window.h"
#include "../../include/Menu/UIComponent.h"
#include <SDL3/SDL.h>

class InventoryCloseListener : public Rml::EventListener {
public:
    InventoryCloseListener(InventoryController* controller) : controller(controller) {}

    void ProcessEvent(Rml::Event&) override {
        controller->hide();
    }

private:
    InventoryController* controller;
};

InventoryController::InventoryController(Window* window, UIComponent* uiComponent)
    : window(window), uiComponent(uiComponent), document(nullptr) {

    slotListener = std::make_unique<InventorySlotListener>(this);

    auto documents = uiComponent->getDocuments();
    if (documents->contains("inventory")) {
        document = documents->at("inventory").get();
        SDL_Log("InventoryController: Found inventory document");

        // Setup close button
        auto closeButton = document->GetElementById("inventory_close");
        if (closeButton) {
            closeButton->AddEventListener(Rml::EventId::Click,
                new class InventoryCloseListener(this));
        }

        setupSlotListeners();
        setupEquipmentSlots();
        document->Hide();
    } else {
        SDL_Log("InventoryController: inventory document NOT found!");
    }
}

InventoryController::~InventoryController() {
    if (document) {
        document->Close();
    }
}

void InventoryController::toggle() {
    if (visible) {
        hide();
    } else {
        show();
    }
}

void InventoryController::show() {
    if (document) {
        document->Show();
        visible = true;
        selectedSlot = -1;  // Clear selection when opening
        selectedEquipmentSlot.clear();
    }
}

void InventoryController::hide() {
    if (document) {
        document->Hide();
        visible = false;
        // Clear selection highlight
        if (selectedSlot != -1) {
            updateSlotHighlight(selectedSlot, false);
            selectedSlot = -1;
        }
        if (!selectedEquipmentSlot.empty()) {
            updateEquipmentSlotHighlight(selectedEquipmentSlot, false);
            selectedEquipmentSlot.clear();
        }
    }
}

bool InventoryController::addItem(std::unique_ptr<Item> item) {
    if (!item) return false;

    // Try to stack with existing item if stackable
    if (item->isStackable()) {
        int existingSlot = findStackableSlot(item.get());
        if (existingSlot != -1) {
            Item* existingItem = items[existingSlot].get();
            int spaceLeft = existingItem->getMaxStackSize() - existingItem->getStackSize();

            if (spaceLeft > 0) {
                int toAdd = std::min(spaceLeft, item->getStackSize());
                existingItem->addToStack(toAdd);

                // If we added everything, we're done
                if (toAdd >= item->getStackSize()) {
                    updateItemDisplay(existingSlot);
                    SDL_Log("Added %d to existing stack in slot %d", toAdd, existingSlot);
                    return true;
                }

                // Otherwise reduce the incoming item's stack
                item->removeFromStack(toAdd);
                updateItemDisplay(existingSlot);
            }
        }
    }

    // Find empty slot for the item (or remaining stack)
    int emptySlot = findEmptySlot();
    if (emptySlot == -1) {
        SDL_Log("Inventory full!");
        return false;
    }

    SDL_Log("Added item '%s' to slot %d", item->getName().c_str(), emptySlot);
    items[emptySlot] = std::move(item);
    updateItemDisplay(emptySlot);

    return true;
}

bool InventoryController::removeItem(int slotIndex, int count) {
    auto it = items.find(slotIndex);
    if (it == items.end()) {
        return false;
    }

    Item* item = it->second.get();

    if (item->isStackable() && item->getStackSize() > count) {
        item->removeFromStack(count);
        updateItemDisplay(slotIndex);
    } else {
        items.erase(it);
        clearSlot(slotIndex);
    }

    return true;
}

bool InventoryController::moveItem(int fromSlot, int toSlot) {
    if (fromSlot == toSlot) return false;

    auto fromIt = items.find(fromSlot);
    if (fromIt == items.end()) return false;

    auto toIt = items.find(toSlot);

    if (toIt == items.end()) {
        // Target slot is empty - move item
        items[toSlot] = std::move(fromIt->second);
        items.erase(fromIt);

        clearSlot(fromSlot);
        updateItemDisplay(toSlot);
        SDL_Log("Moved item from slot %d to slot %d", fromSlot, toSlot);

    } else {
        // Target slot has item - swap
        std::swap(fromIt->second, toIt->second);

        updateItemDisplay(fromSlot);
        updateItemDisplay(toSlot);
        SDL_Log("Swapped items between slot %d and slot %d", fromSlot, toSlot);
    }

    return true;
}

void InventoryController::onSlotClicked(int slotIndex) {
    SDL_Log("Inventory slot %d clicked, selectedSlot = %d, selectedEquipmentSlot = %s",
            slotIndex, selectedSlot, selectedEquipmentSlot.c_str());

    // If an equipment slot is selected, try to move from equipment to inventory
    if (!selectedEquipmentSlot.empty()) {
        auto equipIt = equipmentItems.find(selectedEquipmentSlot);
        if (equipIt != equipmentItems.end()) {
            // Clear equipment slot highlight
            updateEquipmentSlotHighlight(selectedEquipmentSlot, false);
            std::string oldEquipSlot = selectedEquipmentSlot;
            selectedEquipmentSlot.clear();

            // Check if inventory slot has an item
            auto invIt = items.find(slotIndex);
            if (invIt == items.end()) {
                // Empty inventory slot - move equipment item here
                items[slotIndex] = std::move(equipIt->second);
                equipmentItems.erase(equipIt);
                clearEquipmentSlot(oldEquipSlot);
                updateItemDisplay(slotIndex);
                SDL_Log("Moved item from equipment %s to inventory slot %d", oldEquipSlot.c_str(), slotIndex);
            } else {
                // Inventory slot has item - try to swap if valid
                if (canEquipToSlot(oldEquipSlot, invIt->second.get())) {
                    std::swap(equipIt->second, invIt->second);
                    updateEquipmentDisplay(oldEquipSlot);
                    updateItemDisplay(slotIndex);
                    SDL_Log("Swapped items between equipment %s and inventory slot %d", oldEquipSlot.c_str(), slotIndex);
                } else {
                    SDL_Log("Cannot swap - item type doesn't match equipment slot");
                }
            }
        }
        return;
    }

    if (selectedSlot == -1) {
        // No slot selected yet - select this one if it has an item
        if (items.find(slotIndex) != items.end()) {
            selectedSlot = slotIndex;
            updateSlotHighlight(slotIndex, true);
            SDL_Log("Selected inventory slot %d", slotIndex);
        }
    } else {
        // A slot is already selected
        if (selectedSlot == slotIndex) {
            // Clicked same slot - deselect
            updateSlotHighlight(selectedSlot, false);
            selectedSlot = -1;
            SDL_Log("Deselected slot");
        } else {
            // Clicked different slot - move/swap item
            int oldSelected = selectedSlot;
            updateSlotHighlight(selectedSlot, false);
            selectedSlot = -1;

            moveItem(oldSelected, slotIndex);
        }
    }
}

void InventoryController::useItem(int slotIndex, Player* player) {
    auto it = items.find(slotIndex);
    if (it == items.end()) return;

    Item* item = it->second.get();
    item->use(player);

    // If consumable and stack is empty, remove the item
    if (item->getType() == ItemType::CONSUMABLE && item->getStackSize() <= 0) {
        items.erase(it);
        clearSlot(slotIndex);
    } else {
        updateItemDisplay(slotIndex);
    }
}

void InventoryController::updateItemDisplay(int slotIndex) {
    if (!document) return;

    std::string slotId = "slot_" + std::to_string(slotIndex);
    auto slotElement = document->GetElementById(slotId.c_str());
    if (!slotElement) return;

    auto it = items.find(slotIndex);
    if (it == items.end()) return;

    Item* item = it->second.get();

    // Create item visual with icon if available
    std::string itemHTML = "<div class='inventory-item'>";

    // Add icon if path is set
    if (!item->getIconPath().empty()) {
        // RmlUI resolves paths relative to document location (assets/ui/)
        // Icons are stored at "assets/textures/items/..."
        // Use relative path from assets/ui/ to assets/textures/
        std::string iconPath = item->getIconPath();
        if (iconPath.rfind("assets/", 0) == 0) {
            // Go up from assets/ui/ to assets/, then into textures/
            iconPath = "../" + iconPath.substr(7);
        }
        // Also handle paths without assets/ prefix
        else if (iconPath.rfind("textures/", 0) == 0) {
            iconPath = "../" + iconPath;
        }
        itemHTML += "<img class='item-icon' src='" + iconPath + "'/>";
    } else {
        // Fallback to text name if no icon
        itemHTML += "<span class='item-name'>" + item->getName() + "</span>";
    }

    // Show stack count if stackable and more than 1
    if (item->isStackable() && item->getStackSize() > 1) {
        itemHTML += "<span class='item-count'>" + std::to_string(item->getStackSize()) + "</span>";
    }

    itemHTML += "</div>";

    slotElement->SetInnerRML(itemHTML.c_str());
    slotElement->SetClass("occupied", true);
}

void InventoryController::clearSlot(int slotIndex) {
    if (!document) return;

    std::string slotId = "slot_" + std::to_string(slotIndex);
    auto slotElement = document->GetElementById(slotId.c_str());
    if (!slotElement) return;

    slotElement->SetInnerRML("");
    slotElement->SetClass("occupied", false);
    slotElement->SetClass("selected", false);
}

void InventoryController::setupSlotListeners() {
    if (!document) return;

    // Setup click listeners on all slots
    for (int i = 0; i < totalSlots; i++) {
        std::string slotId = "slot_" + std::to_string(i);
        auto slot = document->GetElementById(slotId.c_str());
        if (slot) {
            slot->AddEventListener(Rml::EventId::Click, slotListener.get());
        }
    }
    SDL_Log("InventoryController: Set up click listeners for %d slots", totalSlots);
}

void InventoryController::updateSlotHighlight(int slotIndex, bool selected) {
    if (!document) return;

    std::string slotId = "slot_" + std::to_string(slotIndex);
    auto slotElement = document->GetElementById(slotId.c_str());
    if (slotElement) {
        slotElement->SetClass("selected", selected);
    }
}

int InventoryController::findEmptySlot() {
    for (int i = 0; i < totalSlots; i++) {
        if (items.find(i) == items.end()) {
            return i;
        }
    }
    return -1;
}

int InventoryController::findStackableSlot(Item* item) {
    if (!item || !item->isStackable()) return -1;

    for (auto& [slotIndex, slotItem] : items) {
        if (slotItem->getName() == item->getName() &&
            slotItem->isStackable() &&
            slotItem->getStackSize() < slotItem->getMaxStackSize()) {
            return slotIndex;
        }
    }
    return -1;
}

Item* InventoryController::getItem(int slotIndex) {
    auto it = items.find(slotIndex);
    if (it != items.end()) {
        return it->second.get();
    }
    return nullptr;
}

int InventoryController::countItems(ItemType type) const {
    int count = 0;
    for (const auto& [slotIndex, item] : items) {
        if (item->getType() == type) {
            count += item->getStackSize();
        }
    }
    return count;
}

int InventoryController::countMaterials(MaterialType materialType) const {
    int count = 0;
    for (const auto& [slotIndex, item] : items) {
        if (item->getType() == ItemType::MATERIAL) {
            auto* material = dynamic_cast<Material*>(item.get());
            if (material && material->getMaterialType() == materialType) {
                count += item->getStackSize();
            }
        }
    }
    return count;
}

void InventoryController::setupEquipmentSlots() {
    // Define equipment slot types
    equipmentSlotTypes["slot_helmet"] = EquipmentSlotType::HELMET;
    equipmentSlotTypes["slot_chestplate"] = EquipmentSlotType::CHESTPLATE;
    equipmentSlotTypes["slot_leggings"] = EquipmentSlotType::LEGGINGS;
    equipmentSlotTypes["slot_boots"] = EquipmentSlotType::BOOTS;
    equipmentSlotTypes["slot_amulet_0"] = EquipmentSlotType::AMULET;
    equipmentSlotTypes["slot_amulet_1"] = EquipmentSlotType::AMULET;

    // Add click listeners to equipment slots
    for (const auto& [slotId, slotType] : equipmentSlotTypes) {
        auto slot = document->GetElementById(slotId.c_str());
        if (slot) {
            slot->AddEventListener(Rml::EventId::Click, slotListener.get());
        }
    }
    SDL_Log("InventoryController: Set up %zu equipment slots", equipmentSlotTypes.size());
}

bool InventoryController::isEquipmentSlot(const std::string& slotId) {
    return equipmentSlotTypes.find(slotId) != equipmentSlotTypes.end();
}

bool InventoryController::canEquipToSlot(const std::string& slotId, Item* item) {
    if (!item) return false;

    auto it = equipmentSlotTypes.find(slotId);
    if (it == equipmentSlotTypes.end()) return false;

    EquipmentSlotType slotType = it->second;

    // Check if item type matches slot type
    switch (slotType) {
        case EquipmentSlotType::HELMET:
            if (item->getType() == ItemType::ARMOUR) {
                auto* armour = dynamic_cast<Armour*>(item);
                return armour && armour->getArmourType() == ArmourType::HELMET;
            }
            return false;

        case EquipmentSlotType::CHESTPLATE:
            if (item->getType() == ItemType::ARMOUR) {
                auto* armour = dynamic_cast<Armour*>(item);
                return armour && armour->getArmourType() == ArmourType::CHESTPLATE;
            }
            return false;

        case EquipmentSlotType::LEGGINGS:
            if (item->getType() == ItemType::ARMOUR) {
                auto* armour = dynamic_cast<Armour*>(item);
                return armour && armour->getArmourType() == ArmourType::LEGGINGS;
            }
            return false;

        case EquipmentSlotType::BOOTS:
            if (item->getType() == ItemType::ARMOUR) {
                auto* armour = dynamic_cast<Armour*>(item);
                return armour && armour->getArmourType() == ArmourType::BOOTS;
            }
            return false;

        case EquipmentSlotType::AMULET:
            return item->getType() == ItemType::AMULET;

        case EquipmentSlotType::INVENTORY:
        default:
            return true;  // Anything can go in inventory
    }
}

void InventoryController::onEquipmentSlotClicked(const std::string& slotId) {
    SDL_Log("Equipment slot %s clicked, selectedSlot = %d, selectedEquipmentSlot = %s",
            slotId.c_str(), selectedSlot, selectedEquipmentSlot.c_str());

    // If an inventory slot is selected, try to equip the item
    if (selectedSlot != -1) {
        auto invIt = items.find(selectedSlot);
        if (invIt != items.end()) {
            // Check if the item can go in this equipment slot
            if (canEquipToSlot(slotId, invIt->second.get())) {
                // Clear inventory slot highlight
                updateSlotHighlight(selectedSlot, false);
                int oldInvSlot = selectedSlot;
                selectedSlot = -1;

                // Check if equipment slot already has an item
                auto equipIt = equipmentItems.find(slotId);
                if (equipIt == equipmentItems.end()) {
                    // Empty equipment slot - move inventory item here
                    equipmentItems[slotId] = std::move(invIt->second);
                    items.erase(invIt);
                    clearSlot(oldInvSlot);
                    updateEquipmentDisplay(slotId);
                    SDL_Log("Equipped item from slot %d to %s", oldInvSlot, slotId.c_str());
                } else {
                    // Equipment slot has item - swap
                    std::swap(invIt->second, equipIt->second);
                    updateItemDisplay(oldInvSlot);
                    updateEquipmentDisplay(slotId);
                    SDL_Log("Swapped items between inventory slot %d and equipment %s", oldInvSlot, slotId.c_str());
                }
            } else {
                SDL_Log("Cannot equip - item type doesn't match equipment slot %s", slotId.c_str());
            }
        }
        return;
    }

    // No inventory slot selected - handle equipment slot selection
    if (selectedEquipmentSlot.empty()) {
        // No equipment slot selected yet - select this one if it has an item
        if (equipmentItems.find(slotId) != equipmentItems.end()) {
            selectedEquipmentSlot = slotId;
            updateEquipmentSlotHighlight(slotId, true);
            SDL_Log("Selected equipment slot %s", slotId.c_str());
        }
    } else {
        // An equipment slot is already selected
        if (selectedEquipmentSlot == slotId) {
            // Clicked same slot - deselect
            updateEquipmentSlotHighlight(selectedEquipmentSlot, false);
            selectedEquipmentSlot.clear();
            SDL_Log("Deselected equipment slot");
        } else {
            // Clicked different equipment slot - try to swap if both have items and types match
            auto fromIt = equipmentItems.find(selectedEquipmentSlot);
            auto toIt = equipmentItems.find(slotId);

            updateEquipmentSlotHighlight(selectedEquipmentSlot, false);
            std::string oldEquipSlot = selectedEquipmentSlot;
            selectedEquipmentSlot.clear();

            if (fromIt != equipmentItems.end()) {
                if (toIt == equipmentItems.end()) {
                    // Target slot is empty - check if item type matches
                    if (canEquipToSlot(slotId, fromIt->second.get())) {
                        equipmentItems[slotId] = std::move(fromIt->second);
                        equipmentItems.erase(fromIt);
                        clearEquipmentSlot(oldEquipSlot);
                        updateEquipmentDisplay(slotId);
                        SDL_Log("Moved item from equipment %s to %s", oldEquipSlot.c_str(), slotId.c_str());
                    } else {
                        SDL_Log("Cannot move - item type doesn't match target equipment slot");
                    }
                } else {
                    // Both slots have items - check if swap is valid
                    if (canEquipToSlot(slotId, fromIt->second.get()) &&
                        canEquipToSlot(oldEquipSlot, toIt->second.get())) {
                        std::swap(fromIt->second, toIt->second);
                        updateEquipmentDisplay(oldEquipSlot);
                        updateEquipmentDisplay(slotId);
                        SDL_Log("Swapped items between equipment %s and %s", oldEquipSlot.c_str(), slotId.c_str());
                    } else {
                        SDL_Log("Cannot swap - item types don't match equipment slots");
                    }
                }
            }
        }
    }
}

void InventoryController::updateEquipmentDisplay(const std::string& slotId) {
    if (!document) return;

    auto slotElement = document->GetElementById(slotId.c_str());
    if (!slotElement) return;

    auto it = equipmentItems.find(slotId);
    if (it == equipmentItems.end()) return;

    Item* item = it->second.get();

    // Create item visual with icon if available
    std::string itemHTML = "<div class='inventory-item'>";

    // Add icon if path is set
    if (!item->getIconPath().empty()) {
        std::string iconPath = item->getIconPath();
        if (iconPath.rfind("assets/", 0) == 0) {
            iconPath = "../" + iconPath.substr(7);
        } else if (iconPath.rfind("textures/", 0) == 0) {
            iconPath = "../" + iconPath;
        }
        itemHTML += "<img class='item-icon' src='" + iconPath + "'/>";
    } else {
        itemHTML += "<span class='item-name'>" + item->getName() + "</span>";
    }

    itemHTML += "</div>";

    slotElement->SetInnerRML(itemHTML.c_str());
    slotElement->SetClass("occupied", true);
}

void InventoryController::clearEquipmentSlot(const std::string& slotId) {
    if (!document) return;

    auto slotElement = document->GetElementById(slotId.c_str());
    if (!slotElement) return;

    // Restore the original slot icon based on slot type
    std::string iconHTML;
    auto it = equipmentSlotTypes.find(slotId);
    if (it != equipmentSlotTypes.end()) {
        switch (it->second) {
            case EquipmentSlotType::HELMET:
                iconHTML = "<div class='slot-icon helmet-icon'></div>";
                break;
            case EquipmentSlotType::CHESTPLATE:
                iconHTML = "<div class='slot-icon chestplate-icon'></div>";
                break;
            case EquipmentSlotType::LEGGINGS:
                iconHTML = "<div class='slot-icon leggings-icon'></div>";
                break;
            case EquipmentSlotType::BOOTS:
                iconHTML = "<div class='slot-icon boots-icon'></div>";
                break;
            case EquipmentSlotType::AMULET:
                iconHTML = "<div class='slot-icon amulet-icon'></div>";
                break;
            default:
                break;
        }
    }

    slotElement->SetInnerRML(iconHTML.c_str());
    slotElement->SetClass("occupied", false);
    slotElement->SetClass("selected", false);
}

void InventoryController::updateEquipmentSlotHighlight(const std::string& slotId, bool selected) {
    if (!document) return;

    auto slotElement = document->GetElementById(slotId.c_str());
    if (slotElement) {
        slotElement->SetClass("selected", selected);
    }
}

// Slot Click Listener Implementation
void InventorySlotListener::ProcessEvent(Rml::Event& event) {
    auto element = event.GetCurrentElement();
    if (!element) return;

    // Get slot id from element
    auto id = element->GetId();
    std::string idStr(id.begin(), id.end());

    // Check if this is an equipment slot or inventory slot
    if (idStr == "slot_helmet" || idStr == "slot_chestplate" ||
        idStr == "slot_leggings" || idStr == "slot_boots" ||
        idStr.find("slot_amulet_") == 0) {
        controller->onEquipmentSlotClicked(idStr);
    } else if (idStr.find("slot_") == 0) {
        // Inventory slot (format: "slot_X" where X is a number)
        try {
            int slotIndex = std::stoi(idStr.substr(5));
            controller->onSlotClicked(slotIndex);
        } catch (...) {
            // Not a valid inventory slot number
        }
    }
}
