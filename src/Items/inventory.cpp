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
    SDL_Log("Slot %d clicked, selectedSlot = %d", slotIndex, selectedSlot);

    if (selectedSlot == -1) {
        // No slot selected yet - select this one if it has an item
        if (items.find(slotIndex) != items.end()) {
            selectedSlot = slotIndex;
            updateSlotHighlight(slotIndex, true);
            SDL_Log("Selected slot %d", slotIndex);
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
        // Icons are stored with path "assets/textures/items/..."
        // We need path "textures/items/..." for RmlUI to find them at "assets/ui/textures/items/..."
        std::string iconPath = item->getIconPath();
        // Remove "assets/" prefix - RmlUI will prepend "assets/ui/"
        if (iconPath.rfind("assets/", 0) == 0) {
            iconPath = iconPath.substr(7); // "textures/items/wooden_sword.png"
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

// Slot Click Listener Implementation
void InventorySlotListener::ProcessEvent(Rml::Event& event) {
    auto element = event.GetCurrentElement();
    if (!element) return;

    // Get slot index from element id (format: "slot_X")
    auto id = element->GetId();
    std::string idStr(id.begin(), id.end());

    if (idStr.find("slot_") == 0) {
        int slotIndex = std::stoi(idStr.substr(5));
        controller->onSlotClicked(slotIndex);
    }
}
