//
// Created by honzi on 31. 12. 2025.
//

#include "../../include/Items/inventory.h"
#include "../../include/Items/Crafting.h"
#include "../../include/Items/ChestInventory.h"
#include "../../include/Structures/Chest.h"
#include "../../include/Entities/Player.hpp"

#include "../../include/Window/Window.h"
#include "../../include/Menu/UIComponent.h"
#include <SDL3/SDL.h>

#include "../../include/Entities/Player.hpp"

class InventoryCloseListener : public Rml::EventListener {
public:
    InventoryCloseListener(InventoryController* controller) : controller(controller) {}

    void ProcessEvent(Rml::Event&) override {
        controller->hide();
    }

private:
    InventoryController* controller;
};

class CraftingButtonListener : public Rml::EventListener {
public:
    CraftingButtonListener(InventoryController* controller) : controller(controller) {}

    void ProcessEvent(Rml::Event&) override {
        controller->toggleCraftingUI();
    }

private:
    InventoryController* controller;
};

class CraftingCloseListener : public Rml::EventListener {
public:
    CraftingCloseListener(InventoryController* controller) : controller(controller) {}

    void ProcessEvent(Rml::Event&) override {
        controller->hideCraftingUI();
    }

private:
    InventoryController* controller;
};

class CraftRecipeListener : public Rml::EventListener {
public:
    CraftRecipeListener(InventoryController* controller, const std::string& recipeId)
        : controller(controller), recipeId(recipeId) {}

    void ProcessEvent(Rml::Event&) override {
        controller->craftRecipe(recipeId);
    }

private:
    InventoryController* controller;
    std::string recipeId;
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
        setupCraftingUI();

        // Initialize first hotbar slot as active
        auto firstSlot = document->GetElementById("slot_0");
        if (firstSlot) {
            firstSlot->SetClass("active-hotbar", true);
        }

        document->Hide();
    } else {
        SDL_Log("InventoryController: inventory document NOT found!");
    }

    // Setup crafting document
    if (documents->contains("crafting")) {
        craftingDocument = documents->at("crafting").get();
        SDL_Log("InventoryController: Found crafting document");

        auto craftingCloseButton = craftingDocument->GetElementById("crafting_close");
        if (craftingCloseButton) {
            craftingCloseButton->AddEventListener(Rml::EventId::Click,
                new CraftingCloseListener(this));
        }

        craftingDocument->Hide();
    } else {
        SDL_Log("InventoryController: crafting document NOT found!");
    }

    // Setup hotbar document
    if (documents->contains("hotbar")) {
        hotbarDocument = documents->at("hotbar").get();
        SDL_Log("InventoryController: Found hotbar document");
        setupHotbarUI();
        hotbarDocument->Show();  // Hotbar is always visible during gameplay
    } else {
        SDL_Log("InventoryController: hotbar document NOT found!");
    }
}

InventoryController::~InventoryController() {
    if (document) {
        document->Close();
    }
    if (craftingDocument) {
        craftingDocument->Close();
    }
    if (hotbarDocument) {
        hotbarDocument->Close();
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

void InventoryController::clearSelection() {
    if (selectedSlot != -1) {
        updateSlotHighlight(selectedSlot, false);
        selectedSlot = -1;
    }
    if (!selectedEquipmentSlot.empty()) {
        updateEquipmentSlotHighlight(selectedEquipmentSlot, false);
        selectedEquipmentSlot.clear();
    }
}

std::unique_ptr<Item> InventoryController::takeItem(int slotIndex) {
    auto it = items.find(slotIndex);
    if (it == items.end()) return nullptr;

    std::unique_ptr<Item> item = std::move(it->second);
    items.erase(it);
    clearSlot(slotIndex);

    // Update quickbar if slot is in quickbar range
    if (slotIndex < QUICKBAR_SIZE) {
        updateQuickbarSlot(slotIndex);
    }

    SDL_Log("Took item '%s' from inventory slot %d", item->getName().c_str(), slotIndex);
    return item;
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
                    // Update quickbar if slot is in quickbar range
                    if (existingSlot < QUICKBAR_SIZE) {
                        updateQuickbarSlot(existingSlot);
                        if (existingSlot == selectedQuickbarSlot) {
                            printActiveSlotInfo();
                        }
                    }
                    SDL_Log("Added %d to existing stack in slot %d", toAdd, existingSlot);
                    return true;
                }

                // Otherwise reduce the incoming item's stack
                item->removeFromStack(toAdd);
                updateItemDisplay(existingSlot);
                // Update quickbar if slot is in quickbar range
                if (existingSlot < QUICKBAR_SIZE) {
                    updateQuickbarSlot(existingSlot);
                }
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

    // Update quickbar if slot is in quickbar range
    if (emptySlot < QUICKBAR_SIZE) {
        updateQuickbarSlot(emptySlot);
        if (emptySlot == selectedQuickbarSlot) {
            printActiveSlotInfo();
        }
    }

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

    // Update quickbar if slot is in quickbar range
    if (slotIndex < QUICKBAR_SIZE) {
        updateQuickbarSlot(slotIndex);
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
        // Target slot has item - check if we can stack
        Item* fromItem = fromIt->second.get();
        Item* toItem = toIt->second.get();

        // Check if both items are stackable and the same type
        if (fromItem->isStackable() && toItem->isStackable() &&
            fromItem->getName() == toItem->getName()) {

            int fromAmount = fromItem->getStackSize();
            int toAmount = toItem->getStackSize();
            int maxStack = toItem->getMaxStackSize();
            int spaceInTarget = maxStack - toAmount;

            if (spaceInTarget > 0) {
                // Can combine at least some
                int transferAmount = std::min(fromAmount, spaceInTarget);

                toItem->addToStack(transferAmount);
                fromItem->removeFromStack(transferAmount);

                // Check if source stack is now empty
                if (fromItem->getStackSize() <= 0) {
                    items.erase(fromIt);
                    clearSlot(fromSlot);
                    SDL_Log("Combined stacks: moved all %d from slot %d to slot %d", transferAmount, fromSlot, toSlot);
                } else {
                    updateItemDisplay(fromSlot);
                    SDL_Log("Combined stacks: moved %d from slot %d to slot %d, %d remaining",
                            transferAmount, fromSlot, toSlot, fromItem->getStackSize());
                }
                updateItemDisplay(toSlot);
            } else {
                // Target stack is full - swap instead
                std::swap(fromIt->second, toIt->second);
                updateItemDisplay(fromSlot);
                updateItemDisplay(toSlot);
                SDL_Log("Swapped full stacks between slot %d and slot %d", fromSlot, toSlot);
            }
        } else {
            // Different items or not stackable - swap
            std::swap(fromIt->second, toIt->second);

            updateItemDisplay(fromSlot);
            updateItemDisplay(toSlot);
            SDL_Log("Swapped items between slot %d and slot %d", fromSlot, toSlot);
        }
    }

    // Update quickbar if either slot is in quickbar range
    if (fromSlot < QUICKBAR_SIZE) {
        updateQuickbarSlot(fromSlot);
    }
    if (toSlot < QUICKBAR_SIZE) {
        updateQuickbarSlot(toSlot);
    }

    return true;
}

void InventoryController::onSlotClicked(int slotIndex) {
    SDL_Log("Inventory slot %d clicked, selectedSlot = %d, selectedEquipmentSlot = %s",
            slotIndex, selectedSlot, selectedEquipmentSlot.c_str());

    // Check if chest inventory has a selected item
    if (chestInventoryUI && chestInventoryUI->isVisible() && chestInventoryUI->getSelectedSlot() != -1) {
        int chestSlot = chestInventoryUI->getSelectedSlot();
        SDL_Log("Chest has slot %d selected, transferring to player slot %d", chestSlot, slotIndex);

        Chest* chest = chestInventoryUI->getCurrentChest();
        if (chest) {
            auto* storage = chest->getChestStorage();
            if (storage) {
                auto& chestItems = storage->getItems();
                auto chestIt = chestItems.find(chestSlot);
                if (chestIt != chestItems.end()) {
                    std::unique_ptr<Item> chestItem = std::move(chestIt->second);
                    chestItems.erase(chestIt);

                    auto invIt = items.find(slotIndex);
                    if (invIt == items.end()) {
                        // Empty inventory slot - place item here
                        items[slotIndex] = std::move(chestItem);
                        updateItemDisplay(slotIndex);
                        if (slotIndex < QUICKBAR_SIZE) {
                            updateQuickbarSlot(slotIndex);
                        }
                        SDL_Log("Moved item from chest slot %d to player slot %d", chestSlot, slotIndex);
                    } else {
                        // Inventory slot has item - try to stack or swap
                        Item* existingItem = invIt->second.get();
                        Item* newItem = chestItem.get();

                        if (existingItem->isStackable() && newItem->isStackable() &&
                            existingItem->getName() == newItem->getName()) {
                            // Try to stack
                            int spaceInTarget = existingItem->getMaxStackSize() - existingItem->getStackSize();
                            if (spaceInTarget > 0) {
                                int transferAmount = std::min(newItem->getStackSize(), spaceInTarget);
                                existingItem->addToStack(transferAmount);
                                newItem->removeFromStack(transferAmount);
                                updateItemDisplay(slotIndex);
                                if (slotIndex < QUICKBAR_SIZE) {
                                    updateQuickbarSlot(slotIndex);
                                }

                                if (newItem->getStackSize() > 0) {
                                    // Remaining items go back to chest
                                    chestItems[chestSlot] = std::move(chestItem);
                                }
                                SDL_Log("Stacked items from chest to player slot %d", slotIndex);
                            } else {
                                // Stack full - swap items
                                std::swap(invIt->second, chestItem);
                                updateItemDisplay(slotIndex);
                                if (slotIndex < QUICKBAR_SIZE) {
                                    updateQuickbarSlot(slotIndex);
                                }
                                chestItems[chestSlot] = std::move(chestItem);
                                SDL_Log("Swapped items between player slot %d and chest slot %d", slotIndex, chestSlot);
                            }
                        } else {
                            // Different items - swap
                            std::swap(invIt->second, chestItem);
                            updateItemDisplay(slotIndex);
                            if (slotIndex < QUICKBAR_SIZE) {
                                updateQuickbarSlot(slotIndex);
                            }
                            chestItems[chestSlot] = std::move(chestItem);
                            SDL_Log("Swapped different items between player slot %d and chest slot %d", slotIndex, chestSlot);
                        }
                    }
                }
                // Update chest display for the affected slot
                chestInventoryUI->clearSelection();
            }
        }
        return;
    }

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
                // Update quickbar if slot is in quickbar range
                if (slotIndex < QUICKBAR_SIZE) {
                    updateQuickbarSlot(slotIndex);
                }
                // Update player armour data if armour was unequipped
                updatePlayerArmourData();
                SDL_Log("Moved item from equipment %s to inventory slot %d", oldEquipSlot.c_str(), slotIndex);
            } else {
                // Inventory slot has item - try to swap if valid
                if (canEquipToSlot(oldEquipSlot, invIt->second.get())) {
                    std::swap(equipIt->second, invIt->second);
                    updateEquipmentDisplay(oldEquipSlot);
                    updateItemDisplay(slotIndex);
                    // Update quickbar if slot is in quickbar range
                    if (slotIndex < QUICKBAR_SIZE) {
                        updateQuickbarSlot(slotIndex);
                    }
                    // Update player armour data if armour was swapped
                    updatePlayerArmourData();
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

    // Update quickbar if slot is in quickbar range
    if (slotIndex < QUICKBAR_SIZE) {
        updateQuickbarSlot(slotIndex);
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
                    // Update quickbar if slot is in quickbar range
                    if (oldInvSlot < QUICKBAR_SIZE) {
                        updateQuickbarSlot(oldInvSlot);
                    }
                    // Update player armour data if armour was equipped
                    updatePlayerArmourData();
                    SDL_Log("Equipped item from slot %d to %s", oldInvSlot, slotId.c_str());
                } else {
                    // Equipment slot has item - swap
                    std::swap(invIt->second, equipIt->second);
                    updateItemDisplay(oldInvSlot);
                    updateEquipmentDisplay(slotId);
                    // Update quickbar if slot is in quickbar range
                    if (oldInvSlot < QUICKBAR_SIZE) {
                        updateQuickbarSlot(oldInvSlot);
                    }
                    // Update player armour data if armour was swapped
                    updatePlayerArmourData();
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
                        // Update player armour data (armour moved between equipment slots)
                        updatePlayerArmourData();
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
                        // Update player armour data (armour swapped between equipment slots)
                        updatePlayerArmourData();
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

// Quickbar functionality implementation
std::string InventoryController::generateItemId(Item* item) const {
    if (!item) return "";

    // Generate a unique item ID based on item type and properties
    std::string id;

    switch (item->getType()) {
        case ItemType::WEAPON: {
            auto* weapon = dynamic_cast<Weapon*>(item);
            if (weapon) {
                std::string materialName = ItemFactory::getMaterialName(weapon->getMaterial());
                std::string weaponTypeName;
                switch (weapon->getWeaponType()) {
                    case WeaponType::SWORD: weaponTypeName = "sword"; break;
                    case WeaponType::AXE: weaponTypeName = "axe"; break;
                    case WeaponType::PICKAXE: weaponTypeName = "pickaxe"; break;
                    case WeaponType::BOW: weaponTypeName = "bow"; break;
                }
                id = materialName + "_" + weaponTypeName;
            }
            break;
        }
        case ItemType::ARMOUR: {
            auto* armour = dynamic_cast<Armour*>(item);
            if (armour) {
                std::string materialName = ItemFactory::getMaterialName(armour->getMaterial());
                std::string armourTypeName;
                switch (armour->getArmourType()) {
                    case ArmourType::HELMET: armourTypeName = "helmet"; break;
                    case ArmourType::CHESTPLATE: armourTypeName = "chestplate"; break;
                    case ArmourType::LEGGINGS: armourTypeName = "leggings"; break;
                    case ArmourType::BOOTS: armourTypeName = "boots"; break;
                }
                id = materialName + "_" + armourTypeName;
            }
            break;
        }
        case ItemType::CONSUMABLE: {
            auto* consumable = dynamic_cast<Consumable*>(item);
            if (consumable) {
                switch (consumable->getConsumableType()) {
                    case ConsumableType::HEALTH_POTION: id = "health_potion"; break;
                    case ConsumableType::DAMAGE_BOOST: id = "damage_boost"; break;
                    case ConsumableType::SPEED_BOOST: id = "speed_boost"; break;
                }
            }
            break;
        }
        case ItemType::MATERIAL: {
            auto* material = dynamic_cast<Material*>(item);
            if (material) {
                id = ItemFactory::getMaterialName(material->getMaterialType());
            }
            break;
        }
        case ItemType::AMULET: {
            auto* amulet = dynamic_cast<Amulet*>(item);
            if (amulet) {
                switch (amulet->getAmuletType()) {
                    case AmuletType::SPEED: id = "speed_amulet"; break;
                    case AmuletType::DAMAGE: id = "damage_amulet"; break;
                    case AmuletType::ARMOUR: id = "armour_amulet"; break;
                }
            }
            break;
        }
    }

    // Convert to lowercase for consistency
    for (char& c : id) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }

    return id;
}

void InventoryController::updateQuickbarSlot(int slot) {
    if (slot < 0 || slot >= QUICKBAR_SIZE) return;

    auto it = items.find(slot);
    if (it != items.end() && it->second) {
        Item* item = it->second.get();
        quickbarSlots[slot].hasItem = true;
        quickbarSlots[slot].itemId = generateItemId(item);
        quickbarSlots[slot].itemName = item->getName();
        quickbarSlots[slot].iconPath = item->getIconPath();
        quickbarSlots[slot].stackSize = item->getStackSize();
        quickbarSlots[slot].itemType = item->getType();
    } else {
        quickbarSlots[slot].hasItem = false;
        quickbarSlots[slot].itemId.clear();
        quickbarSlots[slot].itemName.clear();
        quickbarSlots[slot].iconPath.clear();
        quickbarSlots[slot].stackSize = 0;
        quickbarSlots[slot].itemType = ItemType::MATERIAL;
    }

    // Also update the hotbar UI
    updateHotbarSlot(slot);
}

QuickbarSlotInfo InventoryController::getQuickbarSlot(int slot) const {
    if (slot < 0 || slot >= QUICKBAR_SIZE) {
        return QuickbarSlotInfo{};
    }
    return quickbarSlots[slot];
}

void InventoryController::setSelectedQuickbarSlot(int slot) {
    if (slot >= 0 && slot < QUICKBAR_SIZE) {
        int oldSlot = selectedQuickbarSlot;
        selectedQuickbarSlot = slot;

        // Update visual highlighting in inventory UI
        if (document) {
            // Remove highlight from old slot
            if (oldSlot >= 0 && oldSlot < QUICKBAR_SIZE) {
                auto oldElement = document->GetElementById(("slot_" + std::to_string(oldSlot)).c_str());
                if (oldElement) {
                    oldElement->SetClass("active-hotbar", false);
                }
            }
            // Add highlight to new slot
            auto newElement = document->GetElementById(("slot_" + std::to_string(slot)).c_str());
            if (newElement) {
                newElement->SetClass("active-hotbar", true);
            }
        }

        // Update hotbar UI selection
        updateHotbarSelection();

        printActiveSlotInfo();
    }
}

Item* InventoryController::getActiveQuickbarItem() {
    return getItem(selectedQuickbarSlot);
}

std::string InventoryController::printActiveSlotInfo() const {
    std::string info;
    auto it = items.find(selectedQuickbarSlot);

    HandData handData;
    handData.toolType = HandData::NONE;
    handData.damage = 0.0f;

    if (it != items.end() && it->second) {
        info = it->second->getDisplayInfo();
        printf("=== Active Hotbar Slot %d ===\n", selectedQuickbarSlot + 1);
        printf("%s\n", info.c_str());

        Item* item = it->second.get();

        // Check if it's a weapon
        if (item->getType() == ItemType::WEAPON) {
            const auto* weapon = dynamic_cast<const Weapon*>(item);
            if (weapon) {
                handData.damage = static_cast<float>(weapon->getDamage());

                switch (weapon->getWeaponType()) {
                    case WeaponType::PICKAXE:
                        handData.toolType = HandData::PICKAXE;
                        break;
                    case WeaponType::AXE:
                        handData.toolType = HandData::AXE;
                        break;
                    case WeaponType::SWORD:
                        handData.toolType = HandData::SWORD;
                        break;
                    case WeaponType::BOW:
                        handData.toolType = HandData::NONE;
                        break;
                }
            }
        }
        // Check if it's a placeable
        else if (item->getType() == ItemType::PLACEABLE) {
            handData.toolType = HandData::PLACEABLE;
        }

    } else {
        info = "Empty slot";
        printf("=== Active Hotbar Slot %d ===\n", selectedQuickbarSlot + 1);
        printf("%s\n", info.c_str());
    }

    // Update player's hand data
    if (window && window->server) {
        auto* player = dynamic_cast<Player*>(window->server->GetPlayer());
        if (player) {
            player->SetHandData(handData);
        }
    }

    return info;
}

// Equipment access methods
Item* InventoryController::getEquippedItem(const std::string& slotId) {
    auto it = equipmentItems.find(slotId);
    if (it != equipmentItems.end() && it->second) {
        return it->second.get();
    }
    return nullptr;
}

Item* InventoryController::getEquippedHelmet() {
    return getEquippedItem("helmet_slot");
}

Item* InventoryController::getEquippedChestplate() {
    return getEquippedItem("chestplate_slot");
}

Item* InventoryController::getEquippedLeggings() {
    return getEquippedItem("leggings_slot");
}

Item* InventoryController::getEquippedBoots() {
    return getEquippedItem("boots_slot");
}

Item* InventoryController::getEquippedAmulet() {
    return getEquippedItem("amulet_slot");
}

int InventoryController::getTotalArmorDefense() const {
    int totalDefense = 0;

    auto addDefense = [&](const std::string& slotId) {
        auto it = equipmentItems.find(slotId);
        if (it != equipmentItems.end() && it->second) {
            if (it->second->getType() == ItemType::ARMOUR) {
                Armour* armor = dynamic_cast<Armour*>(it->second.get());
                if (armor) {
                    totalDefense += armor->getDefense();
                }
            }
        }
    };

    addDefense("slot_helmet");
    addDefense("slot_chestplate");
    addDefense("slot_leggings");
    addDefense("slot_boots");

    return totalDefense;
}

void InventoryController::updatePlayerArmourData() const {
    if (window && window->server) {
        auto* player = dynamic_cast<Player*>(window->server->GetPlayer());
        if (player) {
            ArmourData armourData;
            armourData.protection = static_cast<float>(getTotalArmorDefense());
            player->SetArmourData(armourData);
            SDL_Log("Updated player armour protection to: %.0f", armourData.protection);
        }
    }
}

// Crafting functionality implementation

// Event listeners for new crafting UI
class CategoryTabListener : public Rml::EventListener {
public:
    CategoryTabListener(InventoryController* controller, const std::string& category)
        : controller(controller), category(category) {}

    void ProcessEvent(Rml::Event&) override {
        controller->setCategory(category);
    }

private:
    InventoryController* controller;
    std::string category;
};

class RecipeSlotListener : public Rml::EventListener {
public:
    RecipeSlotListener(InventoryController* controller, const std::string& recipeId)
        : controller(controller), recipeId(recipeId) {}

    void ProcessEvent(Rml::Event&) override {
        controller->selectRecipe(recipeId);
    }

private:
    InventoryController* controller;
    std::string recipeId;
};

std::string InventoryController::getMaterialIconPath(const std::string& materialId) {
    if (materialId == "wood") return "../textures/items/wooden_planks.svg";
    if (materialId == "stone") return "../textures/items/stone_pile_granite.svg";
    if (materialId == "leather") return "../textures/items/leather_hide_brown.svg";
    if (materialId == "iron") return "../textures/items/metal_iron.svg";
    if (materialId == "steel") return "../textures/items/metal_steel.svg";
    if (materialId == "dragonscale") return "../textures/items/dorsal_scales_purple.svg";
    if (materialId == "gold") return "../textures/items/metal_gold.svg";
    if (materialId == "bronze") return "../textures/items/metal_bronze.svg";
    if (materialId == "copper") return "../textures/items/metal_copper.svg";
    return "../textures/items/wooden_planks.svg";
}

std::string InventoryController::getRecipeOutputIconPath(const std::string& recipeId) {
    // For materials, use the material icon
    if (recipeId == "steel") return "../textures/items/metal_steel.svg";

    // Amulets use SVG format
    if (recipeId == "bronze_necklace" || recipeId == "copper_crown" || recipeId == "gold_ring") {
        return "../textures/items/" + recipeId + ".svg";
    }

    // For crafted items, use the actual item icon (PNG format)
    // Recipe IDs match file names: iron_sword -> iron_sword.png
    size_t underscorePos = recipeId.find('_');
    if (underscorePos == std::string::npos) {
        return getMaterialIconPath(recipeId);
    }

    // Return the actual item icon path
    return "../textures/items/" + recipeId + ".png";
}

std::string InventoryController::formatItemName(const std::string& itemId) {
    std::string result = itemId;
    // Replace underscores with spaces
    for (char& c : result) {
        if (c == '_') c = ' ';
    }
    // Capitalize first letter of each word
    bool capitalizeNext = true;
    for (char& c : result) {
        if (capitalizeNext && c >= 'a' && c <= 'z') {
            c = c - 'a' + 'A';
        }
        capitalizeNext = (c == ' ');
    }
    return result;
}

void InventoryController::setupCraftingUI() {
    if (!document) return;

    // Setup crafting button listener
    auto craftingButton = document->GetElementById("crafting_button");
    if (craftingButton) {
        craftingButton->AddEventListener(Rml::EventId::Click,
            new CraftingButtonListener(this));
    }

    // Initially hide crafting button
    updateCraftingButton();
}

void InventoryController::setNearCraftingTable(bool near) {
    nearCraftingTable = near;
    updateCraftingButton();

    if (!near && craftingVisible) {
        hideCraftingUI();
    }
}

void InventoryController::updateCraftingButton() {
    if (!document) return;

    auto craftingButtonContainer = document->GetElementById("crafting_button_container");
    if (craftingButtonContainer) {
        if (nearCraftingTable) {
            craftingButtonContainer->SetProperty("display", "flex");
        } else {
            craftingButtonContainer->SetProperty("display", "none");
        }
    }
}

void InventoryController::showCraftingUI() {
    if (craftingDocument && nearCraftingTable) {
        // Setup category tab listeners (only once)
        static bool categoryListenersSetup = false;
        if (!categoryListenersSetup && craftingDocument) {
            const char* categories[] = {"all", "weapon", "tool", "armor", "material", "amulet"};
            for (const char* cat : categories) {
                std::string tabId = "tab_" + std::string(cat);
                auto tab = craftingDocument->GetElementById(tabId.c_str());
                if (tab) {
                    tab->AddEventListener(Rml::EventId::Click, new CategoryTabListener(this, cat));
                }
            }
            categoryListenersSetup = true;
        }

        selectedRecipeId.clear();
        currentCategory = "all";
        updateCraftingRecipeGrid();
        updateSelectedRecipeDetails();
        craftingDocument->Show();
        craftingVisible = true;
    }
}

void InventoryController::hideCraftingUI() {
    if (craftingDocument) {
        craftingDocument->Hide();
        craftingVisible = false;
    }
}

void InventoryController::toggleCraftingUI() {
    if (craftingVisible) {
        hideCraftingUI();
    } else {
        showCraftingUI();
    }
}

void InventoryController::setCategory(const std::string& category) {
    if (currentCategory == category) return;

    currentCategory = category;
    selectedRecipeId.clear();

    // Update tab visual state
    if (craftingDocument) {
        const char* categories[] = {"all", "weapon", "tool", "armor", "material", "amulet"};
        for (const char* cat : categories) {
            std::string tabId = "tab_" + std::string(cat);
            auto tab = craftingDocument->GetElementById(tabId.c_str());
            if (tab) {
                tab->SetClass("active", category == cat);
            }
        }
    }

    updateCraftingRecipeGrid();
    updateSelectedRecipeDetails();
}

void InventoryController::selectRecipe(const std::string& recipeId) {
    // Update selection visual
    if (craftingDocument && !selectedRecipeId.empty()) {
        auto oldSlot = craftingDocument->GetElementById(("slot_" + selectedRecipeId).c_str());
        if (oldSlot) {
            oldSlot->SetClass("selected", false);
        }
    }

    selectedRecipeId = recipeId;

    if (craftingDocument && !selectedRecipeId.empty()) {
        auto newSlot = craftingDocument->GetElementById(("slot_" + selectedRecipeId).c_str());
        if (newSlot) {
            newSlot->SetClass("selected", true);
        }
    }

    updateSelectedRecipeDetails();
}

void InventoryController::updateCraftingRecipeGrid() {
    if (!craftingDocument || !craftingSystem) return;

    auto recipeGrid = craftingDocument->GetElementById("recipe_grid");
    if (!recipeGrid) return;

    // Clear existing recipes
    recipeGrid->SetInnerRML("");

    const auto& recipes = craftingSystem->getAllRecipes();

    std::string gridHTML;
    for (const auto& recipe : recipes) {
        // Filter by category
        if (currentCategory != "all") {
            // Map category names (armor vs armour, amulet vs amulets)
            std::string recipeCategory = recipe.category;
            if (recipeCategory == "armour") recipeCategory = "armor";
            if (recipeCategory == "amulets") recipeCategory = "amulet";

            if (recipeCategory != currentCategory) {
                continue;
            }
        }

        bool canCraft = craftingSystem->canCraft(recipe, this);
        std::string iconPath = getRecipeOutputIconPath(recipe.id);

        gridHTML += "<div class='recipe-slot";
        gridHTML += canCraft ? " craftable" : " not-craftable";
        gridHTML += "' id='slot_" + recipe.id + "'>";
        gridHTML += "<img class='recipe-slot-icon' src='" + iconPath + "'/>";
        gridHTML += "</div>";
    }

    recipeGrid->SetInnerRML(gridHTML.c_str());

    // Add click listeners to recipe slots
    for (const auto& recipe : recipes) {
        auto slot = craftingDocument->GetElementById(("slot_" + recipe.id).c_str());
        if (slot) {
            slot->AddEventListener(Rml::EventId::Click, new RecipeSlotListener(this, recipe.id));
        }
    }
}

void InventoryController::updateSelectedRecipeDetails() {
    if (!craftingDocument || !craftingSystem) return;

    auto detailsPanel = craftingDocument->GetElementById("item_details");
    if (!detailsPanel) return;

    if (selectedRecipeId.empty()) {
        detailsPanel->SetInnerRML("<div class='no-selection'>Select an item to craft</div>");
        return;
    }

    const CraftingRecipe* recipe = craftingSystem->getRecipe(selectedRecipeId);
    if (!recipe) {
        detailsPanel->SetInnerRML("<div class='no-selection'>Recipe not found</div>");
        return;
    }

    bool canCraft = craftingSystem->canCraft(*recipe, this);
    std::string iconPath = getRecipeOutputIconPath(recipe->id);
    std::string itemName = formatItemName(recipe->output.itemId);

    std::string html;

    // Header with item icon and name
    html += "<div class='selected-item-header'>";
    html += "<div class='selected-item-icon'><img src='" + iconPath + "'/></div>";
    html += "<div class='selected-item-info'>";
    html += "<div class='selected-item-name'>" + itemName + "</div>";
    html += "</div>";
    html += "</div>";

    // Materials section
    html += "<div class='materials-section'>";
    html += "<div class='materials-title'>Materials Required:</div>";
    html += "<div class='materials-list'>";

    for (const auto& ingredient : recipe->ingredients) {
        std::string matIconPath = getMaterialIconPath(ingredient.itemId);
        std::string matName = formatItemName(ingredient.itemId);

        // Count how many of this material we have
        int haveAmount = countMaterials(
            ingredient.itemId == "wood" ? MaterialType::WOOD :
            ingredient.itemId == "stone" ? MaterialType::STONE :
            ingredient.itemId == "leather" ? MaterialType::LEATHER :
            ingredient.itemId == "iron" ? MaterialType::IRON :
            ingredient.itemId == "steel" ? MaterialType::STEEL :
            ingredient.itemId == "dragonscale" ? MaterialType::DRAGONSCALE :
            ingredient.itemId == "gold" ? MaterialType::GOLD :
            ingredient.itemId == "bronze" ? MaterialType::BRONZE :
            ingredient.itemId == "copper" ? MaterialType::COPPER :
            MaterialType::NONE
        );

        bool sufficient = haveAmount >= ingredient.amount;

        html += "<div class='material-row " + std::string(sufficient ? "sufficient" : "insufficient") + "'>";
        html += "<img class='material-icon' src='" + matIconPath + "'/>";
        html += "<span class='material-name'>" + matName + "</span>";
        html += "<span class='material-count " + std::string(sufficient ? "sufficient" : "insufficient") + "'>";
        html += std::to_string(haveAmount) + "/" + std::to_string(ingredient.amount);
        html += "</span>";
        html += "</div>";
    }

    html += "</div>";
    html += "</div>";

    // Craft button
    html += "<div class='craft-section'>";
    html += "<div class='craft-button" + std::string(canCraft ? "" : " disabled") + "' id='craft_selected'>Craft</div>";
    html += "</div>";

    detailsPanel->SetInnerRML(html.c_str());

    // Add click listener to craft button
    auto craftButton = craftingDocument->GetElementById("craft_selected");
    if (craftButton) {
        craftButton->AddEventListener(Rml::EventId::Click, new CraftRecipeListener(this, selectedRecipeId));
    }
}

void InventoryController::craftRecipe(const std::string& recipeId) {
    if (!craftingSystem) return;

    const CraftingRecipe* recipe = craftingSystem->getRecipe(recipeId);
    if (!recipe) return;

    if (!craftingSystem->canCraft(*recipe, this)) {
        SDL_Log("Cannot craft %s - missing ingredients", recipeId.c_str());
        return;
    }

    auto craftedItem = craftingSystem->craft(*recipe, this);
    if (craftedItem) {
        if (addItem(std::move(craftedItem))) {
            SDL_Log("Crafted and added %s to inventory", recipeId.c_str());
        } else {
            SDL_Log("Inventory full - could not add crafted item");
        }
    }

    // Refresh the UI to update craftable status and material counts
    updateCraftingRecipeGrid();
    updateSelectedRecipeDetails();
}

// Hotbar UI implementation
void InventoryController::setupHotbarUI() {
    if (!hotbarDocument) return;

    // Initialize all hotbar slots
    updateAllHotbarSlots();
    updateHotbarSelection();

    SDL_Log("InventoryController: Hotbar UI setup complete");
}

void InventoryController::updateHotbarSlot(int slot) {
    if (!hotbarDocument || slot < 0 || slot >= QUICKBAR_SIZE) return;

    std::string slotId = "hotbar_" + std::to_string(slot);
    auto slotElement = hotbarDocument->GetElementById(slotId.c_str());
    if (!slotElement) return;

    auto it = items.find(slot);
    if (it != items.end() && it->second) {
        Item* item = it->second.get();

        // Create item visual with icon
        std::string itemHTML = "<div class='hotbar-item'>";

        if (!item->getIconPath().empty()) {
            std::string iconPath = item->getIconPath();
            // Adjust path for RmlUI (relative to assets/ui/)
            if (iconPath.rfind("assets/", 0) == 0) {
                iconPath = "../" + iconPath.substr(7);
            } else if (iconPath.rfind("textures/", 0) == 0) {
                iconPath = "../" + iconPath;
            }
            itemHTML += "<img class='item-icon' src='" + iconPath + "'/>";
        }

        // Show stack count if stackable and more than 1
        if (item->isStackable() && item->getStackSize() > 1) {
            itemHTML += "<span class='item-count'>" + std::to_string(item->getStackSize()) + "</span>";
        }

        itemHTML += "</div>";

        slotElement->SetInnerRML(itemHTML.c_str());
        slotElement->SetClass("occupied", true);
    } else {
        // Empty slot
        slotElement->SetInnerRML("");
        slotElement->SetClass("occupied", false);
    }
}

void InventoryController::updateAllHotbarSlots() {
    for (int i = 0; i < QUICKBAR_SIZE; i++) {
        updateHotbarSlot(i);
    }
}

void InventoryController::updateHotbarSelection() {
    if (!hotbarDocument) return;

    // Update selection highlight on all hotbar slots
    for (int i = 0; i < QUICKBAR_SIZE; i++) {
        std::string slotId = "hotbar_" + std::to_string(i);
        auto slotElement = hotbarDocument->GetElementById(slotId.c_str());
        if (slotElement) {
            slotElement->SetClass("selected", i == selectedQuickbarSlot);
        }
    }
}

void InventoryController::showHotbar() {
    if (hotbarDocument) {
        hotbarDocument->Show();
    }
}

void InventoryController::hideHotbar() {
    if (hotbarDocument) {
        hotbarDocument->Hide();
    }
}

std::vector<std::pair<int, ItemData>> InventoryController::serializeInventory() const {
    std::vector<std::pair<int, ItemData>> result;
    for (const auto& [slot, item] : items) {
        if (item) {
            result.emplace_back(slot, ItemFactory::serializeItem(item.get()));
        }
    }
    return result;
}

std::vector<std::pair<std::string, ItemData>> InventoryController::serializeEquipment() const {
    std::vector<std::pair<std::string, ItemData>> result;
    for (const auto& [slotId, item] : equipmentItems) {
        if (item) {
            result.emplace_back(slotId, ItemFactory::serializeItem(item.get()));
        }
    }
    return result;
}

void InventoryController::deserializeInventory(const std::vector<std::pair<int, ItemData>>& data) {
    for (const auto& [slot, itemData] : data) {
        auto item = ItemFactory::deserializeItem(itemData);
        if (item && slot >= 0 && slot < totalSlots) {
            items[slot] = std::move(item);
            updateItemDisplay(slot);
            if (slot < QUICKBAR_SIZE) {
                updateQuickbarSlot(slot);
            }
        }
    }
}

void InventoryController::deserializeEquipment(const std::vector<std::pair<std::string, ItemData>>& data) {
    for (const auto& [slotId, itemData] : data) {
        auto item = ItemFactory::deserializeItem(itemData);
        if (item && equipmentSlotTypes.contains(slotId)) {
            equipmentItems[slotId] = std::move(item);
            updateEquipmentDisplay(slotId);
        }
    }
    // Update player armour data after loading equipment
    updatePlayerArmourData();
}

void InventoryController::clearInventory() {
    items.clear();
    equipmentItems.clear();
    for (int i = 0; i < totalSlots; i++) {
        clearSlot(i);
    }
    for (const auto& [slotId, _] : equipmentSlotTypes) {
        clearEquipmentSlot(slotId);
    }
    for (int i = 0; i < QUICKBAR_SIZE; i++) {
        updateQuickbarSlot(i);
    }
}
