//
// Created by honzi on 31. 12. 2025.
//

#ifndef ULTIMATNIHRA_INVENTORY_H
#define ULTIMATNIHRA_INVENTORY_H

#pragma once
#include <RmlUi/Core.h>
#include <unordered_map>
#include <map>
#include <string>
#include <memory>
#include "Item.h"

// Equipment slot types for validation
enum class EquipmentSlotType {
    HELMET,
    CHESTPLATE,
    LEGGINGS,
    BOOTS,
    AMULET,
    INVENTORY  // Regular inventory slot
};

class UIComponent;
class Window;

class InventorySlotListener : public Rml::EventListener {
public:
    InventorySlotListener(class InventoryController* controller)
        : controller(controller) {}

    void ProcessEvent(Rml::Event& event) override;

private:
    InventoryController* controller;
};

class InventoryController {
public:
    InventoryController(Window* window, UIComponent* uiComponent);
    ~InventoryController();

    void toggle();
    void show();
    void hide();
    bool isVisible() const { return visible; }

    // Item management - now uses Item class
    bool addItem(std::unique_ptr<Item> item);
    bool removeItem(int slotIndex, int count = 1);
    bool moveItem(int fromSlot, int toSlot);
    void updateItemDisplay(int slotIndex);
    void clearSlot(int slotIndex);

    // Use item in slot
    void useItem(int slotIndex, Player* player);

    // Click-based item moving
    void onSlotClicked(int slotIndex);
    void onEquipmentSlotClicked(const std::string& slotId);
    int getSelectedSlot() const { return selectedSlot; }
    std::string getSelectedEquipmentSlot() const { return selectedEquipmentSlot; }

    // Get item at slot (returns nullptr if empty)
    Item* getItem(int slotIndex);

    // Get total item count of a specific type
    int countItems(ItemType type) const;

private:
    Window* window;
    UIComponent* uiComponent;
    Rml::ElementDocument* document;
    std::unique_ptr<InventorySlotListener> slotListener;

    std::unordered_map<int, std::unique_ptr<Item>> items; // slot index -> item
    int totalSlots = 20;
    bool visible = false;

    int selectedSlot = -1;  // Currently selected inventory slot for moving
    std::string selectedEquipmentSlot;  // Currently selected equipment slot

    // Equipment slot management
    std::unordered_map<std::string, std::unique_ptr<Item>> equipmentItems; // slot_id -> item
    std::map<std::string, EquipmentSlotType> equipmentSlotTypes;

    void setupSlotListeners();
    void setupEquipmentSlots();
    void updateSlotHighlight(int slotIndex, bool selected);
    void updateEquipmentSlotHighlight(const std::string& slotId, bool selected);
    int findEmptySlot();
    int findStackableSlot(Item* item); // Find slot with same stackable item

    // Equipment slot validation
    bool canEquipToSlot(const std::string& slotId, Item* item);
    bool isEquipmentSlot(const std::string& slotId);
    void updateEquipmentDisplay(const std::string& slotId);
    void clearEquipmentSlot(const std::string& slotId);
};



#endif //ULTIMATNIHRA_INVENTORY_H
