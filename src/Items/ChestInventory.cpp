//
// Created for chest inventory management
//

#include "../../include/Items/ChestInventory.h"
#include "../../include/Items/inventory.h"
#include "../../include/Structures/Chest.h"
#include "../../include/Window/Window.h"
#include "../../include/Menu/UIComponent.h"
#include <SDL3/SDL.h>

// ChestStorage implementation
ChestStorage::ChestStorage(int chestId) : chestId(chestId) {}

bool ChestStorage::addItem(std::unique_ptr<Item> item) {
    if (!item) return false;

    if (item->isStackable()) {
        int existingSlot = findStackableSlot(item.get());
        if (existingSlot != -1) {
            Item* existingItem = items[existingSlot].get();
            int spaceLeft = existingItem->getMaxStackSize() - existingItem->getStackSize();

            if (spaceLeft > 0) {
                int toAdd = std::min(spaceLeft, item->getStackSize());
                existingItem->addToStack(toAdd);

                if (toAdd >= item->getStackSize()) {
                    return true;
                }
                item->removeFromStack(toAdd);
            }
        }
    }

    int emptySlot = findEmptySlot();
    if (emptySlot == -1) {
        return false;
    }

    items[emptySlot] = std::move(item);
    return true;
}

bool ChestStorage::removeItem(int slotIndex, int count) {
    auto it = items.find(slotIndex);
    if (it == items.end()) return false;

    Item* item = it->second.get();
    if (item->isStackable() && item->getStackSize() > count) {
        item->removeFromStack(count);
    } else {
        items.erase(it);
    }
    return true;
}

Item* ChestStorage::getItem(int slotIndex) {
    auto it = items.find(slotIndex);
    return (it != items.end()) ? it->second.get() : nullptr;
}

int ChestStorage::findEmptySlot() {
    for (int i = 0; i < CHEST_SLOTS; i++) {
        if (items.find(i) == items.end()) {
            return i;
        }
    }
    return -1;
}

int ChestStorage::findStackableSlot(Item* item) {
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

// ChestSlotListener implementation
void ChestSlotListener::ProcessEvent(Rml::Event& event) {
    auto element = event.GetCurrentElement();
    if (!element) return;

    auto id = element->GetId();
    std::string idStr(id.begin(), id.end());

    // Extract slot number from ID like "chest_slot_5"
    size_t lastUnderscore = idStr.rfind('_');
    if (lastUnderscore != std::string::npos) {
        try {
            int slotIndex = std::stoi(idStr.substr(lastUnderscore + 1));
            ui->onSlotClicked(slotIndex);
        } catch (...) {
            SDL_Log("ChestSlotListener: Failed to parse slot index from %s", idStr.c_str());
        }
    }
}

// ChestInventoryUI implementation
ChestInventoryUI::ChestInventoryUI(Window* window, UIComponent* uiComponent)
    : window(window), uiComponent(uiComponent), document(nullptr) {

    slotListener = std::make_unique<ChestSlotListener>(this);

    auto documents = uiComponent->getDocuments();
    if (documents->contains("chest_inventory")) {
        document = documents->at("chest_inventory").get();
        SDL_Log("ChestInventoryUI: Found chest_inventory document");
        setupSlotListeners();
        document->Hide();
    } else {
        SDL_Log("ChestInventoryUI: chest_inventory document NOT found!");
    }
}

ChestInventoryUI::~ChestInventoryUI() {
    if (document && visible) {
        document->Hide();
    }
}

void ChestInventoryUI::setupSlotListeners() {
    if (!document) return;

    for (int i = 0; i < CHEST_SLOTS; i++) {
        std::string slotId = "chest_slot_" + std::to_string(i);
        auto slot = document->GetElementById(slotId.c_str());
        if (slot) {
            slot->AddEventListener(Rml::EventId::Click, slotListener.get());
        }
    }
    SDL_Log("ChestInventoryUI: Set up click listeners for %d slots", CHEST_SLOTS);
}

void ChestInventoryUI::openChest(Chest* chest) {
    if (!document || !chest) return;

    currentChest = chest;
    refreshDisplay();
    document->Show();
    visible = true;
    SDL_Log("ChestInventoryUI: Opened chest %d", chest->GetId());
}

void ChestInventoryUI::closeChest() {
    if (!document) return;

    if (selectedSlot != -1) {
        updateSlotHighlight(selectedSlot, false);
        selectedSlot = -1;
    }

    document->Hide();
    visible = false;
    currentChest = nullptr;
    SDL_Log("ChestInventoryUI: Closed");
}

void ChestInventoryUI::refreshDisplay() {
    if (!currentChest || !document) return;

    auto* storage = currentChest->getChestStorage();
    if (!storage) return;

    for (int i = 0; i < CHEST_SLOTS; i++) {
        if (storage->getItem(i)) {
            updateItemDisplay(i);
        } else {
            clearSlot(i);
        }
    }
}

void ChestInventoryUI::updateItemDisplay(int slotIndex) {
    if (!document || !currentChest) return;
    if (slotIndex < 0 || slotIndex >= CHEST_SLOTS) return;

    auto* storage = currentChest->getChestStorage();
    if (!storage) return;

    std::string slotId = "chest_slot_" + std::to_string(slotIndex);
    auto slotElement = document->GetElementById(slotId.c_str());
    if (!slotElement) return;

    Item* item = storage->getItem(slotIndex);
    if (!item) {
        clearSlot(slotIndex);
        return;
    }

    std::string itemHTML = "<div class='inventory-item'>";

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

    if (item->isStackable() && item->getStackSize() > 1) {
        itemHTML += "<span class='item-count'>" + std::to_string(item->getStackSize()) + "</span>";
    }

    itemHTML += "</div>";

    try {
        slotElement->SetInnerRML(itemHTML.c_str());
        slotElement->SetClass("occupied", true);
    } catch (const std::exception& e) {
        SDL_Log("ChestInventoryUI: Error setting slot %d content: %s", slotIndex, e.what());
    }
}

void ChestInventoryUI::clearSlot(int slotIndex) {
    if (!document) return;
    if (slotIndex < 0 || slotIndex >= CHEST_SLOTS) return;

    std::string slotId = "chest_slot_" + std::to_string(slotIndex);
    auto slotElement = document->GetElementById(slotId.c_str());
    if (!slotElement) return;

    try {
        slotElement->SetInnerRML("");
        slotElement->SetClass("occupied", false);
        slotElement->SetClass("selected", false);
    } catch (const std::exception& e) {
        SDL_Log("ChestInventoryUI: Error clearing slot %d: %s", slotIndex, e.what());
    }
}

void ChestInventoryUI::updateSlotHighlight(int slotIndex, bool selected) {
    if (!document) return;

    std::string slotId = "chest_slot_" + std::to_string(slotIndex);
    auto slotElement = document->GetElementById(slotId.c_str());
    if (slotElement) {
        slotElement->SetClass("selected", selected);
    }
}

void ChestInventoryUI::onSlotClicked(int slotIndex) {
    if (!currentChest) return;

    auto* storage = currentChest->getChestStorage();
    if (!storage) return;

    SDL_Log("ChestInventoryUI: Slot %d clicked, selectedSlot = %d", slotIndex, selectedSlot);

    // Check if player inventory has a selected item
    if (inventoryController && inventoryController->getSelectedSlot() != -1) {
        int playerSlot = inventoryController->getSelectedSlot();
        SDL_Log("ChestInventoryUI: Player has slot %d selected, transferring to chest slot %d", playerSlot, slotIndex);

        // Take item from player inventory
        std::unique_ptr<Item> playerItem = inventoryController->takeItem(playerSlot);
        if (playerItem) {
            auto& chestItems = storage->getItems();
            auto existingIt = chestItems.find(slotIndex);

            if (existingIt == chestItems.end()) {
                // Empty chest slot - place item here
                chestItems[slotIndex] = std::move(playerItem);
                updateItemDisplay(slotIndex);
                SDL_Log("ChestInventoryUI: Moved item from player slot %d to chest slot %d", playerSlot, slotIndex);
            } else {
                // Chest slot has item - try to stack or swap
                Item* chestItem = existingIt->second.get();
                Item* newItem = playerItem.get();

                if (chestItem->isStackable() && newItem->isStackable() &&
                    chestItem->getName() == newItem->getName()) {
                    // Try to stack
                    int spaceInTarget = chestItem->getMaxStackSize() - chestItem->getStackSize();
                    if (spaceInTarget > 0) {
                        int transferAmount = std::min(newItem->getStackSize(), spaceInTarget);
                        chestItem->addToStack(transferAmount);
                        newItem->removeFromStack(transferAmount);
                        updateItemDisplay(slotIndex);

                        if (newItem->getStackSize() > 0) {
                            // Remaining items go back to player
                            inventoryController->addItem(std::move(playerItem));
                        }
                        SDL_Log("ChestInventoryUI: Stacked items from player to chest slot %d", slotIndex);
                    } else {
                        // Stack full - swap items
                        std::swap(existingIt->second, playerItem);
                        updateItemDisplay(slotIndex);
                        inventoryController->addItem(std::move(playerItem));
                        SDL_Log("ChestInventoryUI: Swapped items between player and chest slot %d", slotIndex);
                    }
                } else {
                    // Different items - swap
                    std::swap(existingIt->second, playerItem);
                    updateItemDisplay(slotIndex);
                    inventoryController->addItem(std::move(playerItem));
                    SDL_Log("ChestInventoryUI: Swapped different items between player and chest slot %d", slotIndex);
                }
            }
        }
        inventoryController->clearSelection();
        return;
    }

    // Normal chest slot selection logic
    if (selectedSlot == -1) {
        if (storage->getItem(slotIndex)) {
            selectedSlot = slotIndex;
            updateSlotHighlight(slotIndex, true);
        }
    } else {
        if (selectedSlot == slotIndex) {
            updateSlotHighlight(selectedSlot, false);
            selectedSlot = -1;
        } else {
            int oldSelected = selectedSlot;
            updateSlotHighlight(selectedSlot, false);
            selectedSlot = -1;
            moveItem(oldSelected, slotIndex);
        }
    }
}

bool ChestInventoryUI::moveItem(int fromSlot, int toSlot) {
    if (!currentChest || fromSlot == toSlot) return false;

    auto* storage = currentChest->getChestStorage();
    if (!storage) return false;

    auto& items = storage->getItems();
    auto fromIt = items.find(fromSlot);
    if (fromIt == items.end()) return false;

    auto toIt = items.find(toSlot);

    if (toIt == items.end()) {
        items[toSlot] = std::move(fromIt->second);
        items.erase(fromIt);
        clearSlot(fromSlot);
        updateItemDisplay(toSlot);
    } else {
        Item* fromItem = fromIt->second.get();
        Item* toItem = toIt->second.get();

        if (fromItem->isStackable() && toItem->isStackable() &&
            fromItem->getName() == toItem->getName()) {

            int spaceInTarget = toItem->getMaxStackSize() - toItem->getStackSize();
            if (spaceInTarget > 0) {
                int transferAmount = std::min(fromItem->getStackSize(), spaceInTarget);
                toItem->addToStack(transferAmount);
                fromItem->removeFromStack(transferAmount);

                if (fromItem->getStackSize() <= 0) {
                    items.erase(fromIt);
                    clearSlot(fromSlot);
                } else {
                    updateItemDisplay(fromSlot);
                }
                updateItemDisplay(toSlot);
            } else {
                std::swap(fromIt->second, toIt->second);
                updateItemDisplay(fromSlot);
                updateItemDisplay(toSlot);
            }
        } else {
            std::swap(fromIt->second, toIt->second);
            updateItemDisplay(fromSlot);
            updateItemDisplay(toSlot);
        }
    }

    return true;
}

void ChestInventoryUI::clearSelection() {
    if (selectedSlot != -1) {
        updateSlotHighlight(selectedSlot, false);
        selectedSlot = -1;
    }
    // Refresh the display to ensure UI is up to date
    refreshDisplay();
}

bool ChestInventoryUI::transferToPlayerInventory(int chestSlot) {
    if (!currentChest || !inventoryController) return false;

    auto* storage = currentChest->getChestStorage();
    if (!storage) return false;

    auto& chestItems = storage->getItems();
    auto it = chestItems.find(chestSlot);
    if (it == chestItems.end()) return false;

    // Take the item from chest
    std::unique_ptr<Item> item = std::move(it->second);
    chestItems.erase(it);

    // Try to add to player inventory
    if (inventoryController->addItem(std::move(item))) {
        clearSlot(chestSlot);
        SDL_Log("Transferred item from chest slot %d to player inventory", chestSlot);
        return true;
    } else {
        // Failed to add - put it back in chest
        chestItems[chestSlot] = std::move(item);
        SDL_Log("Failed to transfer - player inventory full");
        return false;
    }
}

bool ChestInventoryUI::transferFromPlayerInventory(int chestSlot, std::unique_ptr<Item> item) {
    if (!currentChest || !item) return false;

    auto* storage = currentChest->getChestStorage();
    if (!storage) return false;

    auto& chestItems = storage->getItems();
    auto existingIt = chestItems.find(chestSlot);

    if (existingIt == chestItems.end()) {
        // Empty slot - place item here
        chestItems[chestSlot] = std::move(item);
        updateItemDisplay(chestSlot);
        SDL_Log("Transferred item from player inventory to chest slot %d", chestSlot);
        return true;
    } else {
        // Slot has item - try to stack or swap
        Item* existingItem = existingIt->second.get();
        Item* newItem = item.get();

        if (existingItem->isStackable() && newItem->isStackable() &&
            existingItem->getName() == newItem->getName()) {
            // Try to stack
            int spaceInTarget = existingItem->getMaxStackSize() - existingItem->getStackSize();
            if (spaceInTarget > 0) {
                int transferAmount = std::min(newItem->getStackSize(), spaceInTarget);
                existingItem->addToStack(transferAmount);
                newItem->removeFromStack(transferAmount);
                updateItemDisplay(chestSlot);

                if (newItem->getStackSize() <= 0) {
                    // All items transferred
                    return true;
                }
                // Some items remaining - caller needs to handle
                return false;
            }
        }
        // Cannot stack - swap items handled by caller
        return false;
    }
}
