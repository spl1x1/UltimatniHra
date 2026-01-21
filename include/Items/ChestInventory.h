//
// Created for chest inventory management
//

#ifndef ULTIMATNIHRA_CHESTINVENTORY_H
#define ULTIMATNIHRA_CHESTINVENTORY_H

#pragma once
#include <RmlUi/Core.h>
#include <unordered_map>
#include <string>
#include <memory>
#include "Item.h"

class UIComponent;
class Window;
class Chest;
class InventoryController;

// Data storage for a chest's items (no UI)
class ChestStorage {
public:
    static constexpr int CHEST_SLOTS = 20;

    ChestStorage(int chestId);

    bool addItem(std::unique_ptr<Item> item);
    bool removeItem(int slotIndex, int count = 1);
    Item* getItem(int slotIndex);
    std::unordered_map<int, std::unique_ptr<Item>>& getItems() { return items; }
    int getChestId() const { return chestId; }

private:
    int chestId;
    std::unordered_map<int, std::unique_ptr<Item>> items;

    int findEmptySlot();
    int findStackableSlot(Item* item);
};

// UI controller for displaying chest inventory (singleton per game)
class ChestSlotListener : public Rml::EventListener {
public:
    ChestSlotListener(class ChestInventoryUI* ui) : ui(ui) {}
    void ProcessEvent(Rml::Event& event) override;
private:
    ChestInventoryUI* ui;
};

class ChestInventoryUI {
public:
    ChestInventoryUI(Window* window, UIComponent* uiComponent);
    ~ChestInventoryUI();

    void openChest(Chest* chest);
    void closeChest();
    bool isVisible() const { return visible; }
    Chest* getCurrentChest() const { return currentChest; }

    // Called by slot listener
    void onSlotClicked(int slotIndex);

    // Cross-inventory transfer support
    void setInventoryController(InventoryController* controller) { inventoryController = controller; }
    int getSelectedSlot() const { return selectedSlot; }
    void clearSelection();

    // Transfer items between inventories
    bool transferToPlayerInventory(int chestSlot);
    bool transferFromPlayerInventory(int chestSlot, std::unique_ptr<Item> item);

    static constexpr int CHEST_SLOTS = 20;

private:
    Window* window;
    UIComponent* uiComponent;
    Rml::ElementDocument* document;
    std::unique_ptr<ChestSlotListener> slotListener;
    InventoryController* inventoryController = nullptr;

    Chest* currentChest = nullptr;
    bool visible = false;
    int selectedSlot = -1;

    void setupSlotListeners();
    void refreshDisplay();
    void updateItemDisplay(int slotIndex);
    void clearSlot(int slotIndex);
    void updateSlotHighlight(int slotIndex, bool selected);
    bool moveItem(int fromSlot, int toSlot);
};

#endif //ULTIMATNIHRA_CHESTINVENTORY_H
