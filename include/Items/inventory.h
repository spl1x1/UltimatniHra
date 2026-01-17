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
#include <array>
#include "Item.h"

class CraftingSystem;

// Quickbar slot info for displaying items in the HUD quickbar
struct QuickbarSlotInfo {
    bool hasItem = false;
    std::string itemId;      // Unique identifier for the item (e.g., "iron_sword", "health_potion")
    std::string itemName;    // Display name
    std::string iconPath;    // Path to the item's icon texture
    int stackSize = 0;       // Current stack size (1 for non-stackable items)
    ItemType itemType = ItemType::MATERIAL;  // Type of item in slot
};

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

    // Count materials of a specific material type
    int countMaterials(MaterialType materialType) const;

    // Get all items (for crafting system)
    const std::unordered_map<int, std::unique_ptr<Item>>& getItems() const { return items; }

    // Quickbar functionality (slots 0-4)
    static constexpr int QUICKBAR_SIZE = 5;
    const std::array<QuickbarSlotInfo, QUICKBAR_SIZE>& getQuickbarSlots() const { return quickbarSlots; }
    QuickbarSlotInfo getQuickbarSlot(int slot) const;
    int getSelectedQuickbarSlot() const { return selectedQuickbarSlot; }
    void setSelectedQuickbarSlot(int slot);  // Select quickbar slot (0-4)
    Item* getActiveQuickbarItem();  // Get item at currently selected quickbar slot

    // Crafting functionality
    void setCraftingSystem(CraftingSystem* system) { craftingSystem = system; }
    void setNearCraftingTable(bool near);
    bool isNearCraftingTable() const { return nearCraftingTable; }
    void showCraftingUI();
    void hideCraftingUI();
    void toggleCraftingUI();
    bool isCraftingUIVisible() const { return craftingVisible; }
    void craftRecipe(const std::string& recipeId);

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

    // Quickbar management
    std::array<QuickbarSlotInfo, 5> quickbarSlots;
    int selectedQuickbarSlot = 0;  // Currently selected quickbar slot (0-4)
    void updateQuickbarSlot(int slot);  // Update quickbar info when inventory slot changes
    std::string generateItemId(Item* item) const;  // Generate unique item ID from item

    // Crafting
    CraftingSystem* craftingSystem = nullptr;
    Rml::ElementDocument* craftingDocument = nullptr;
    bool nearCraftingTable = false;
    bool craftingVisible = false;
    void setupCraftingUI();
    void updateCraftingButton();
    void updateCraftingRecipeList();
};



#endif //ULTIMATNIHRA_INVENTORY_H
