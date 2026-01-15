//
// Created by Jar Jar Banton on 15. 1. 2026.
//

#ifndef ULTIMATNIHRA_CRAFTING_H
#define ULTIMATNIHRA_CRAFTING_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include "Item.h"

class InventoryController;

// Represents a single ingredient in a recipe
struct CraftingIngredient {
    std::string itemId;
    int amount;
};

// Represents the output of a recipe
struct CraftingOutput {
    std::string itemId;
    int amount;
};

// Represents a complete crafting recipe
struct CraftingRecipe {
    std::string id;
    std::string category;      // "weapon", "armor", "tool", "material"
    std::string station;       // Optional: required crafting station (e.g., "smelter")
    std::vector<CraftingIngredient> ingredients;
    CraftingOutput output;
};

class CraftingSystem {
public:
    CraftingSystem();
    ~CraftingSystem() = default;

    // Load recipes from JSON file
    bool loadRecipes(const std::string& filePath);

    // Get all available recipes
    [[nodiscard]] const std::vector<CraftingRecipe>& getAllRecipes() const { return recipes; }

    // Get recipes by category
    [[nodiscard]] std::vector<const CraftingRecipe*> getRecipesByCategory(const std::string& category) const;

    // Get a specific recipe by id
    [[nodiscard]] const CraftingRecipe* getRecipe(const std::string& recipeId) const;

    // Check if a recipe can be crafted with current inventory
    [[nodiscard]] bool canCraft(const CraftingRecipe& recipe, InventoryController* inventory) const;

    // Attempt to craft an item - returns the crafted item or nullptr if failed
    std::unique_ptr<Item> craft(const CraftingRecipe& recipe, InventoryController* inventory);

    // Get craftable recipes based on current inventory
    [[nodiscard]] std::vector<const CraftingRecipe*> getCraftableRecipes(InventoryController* inventory) const;

    // Get all registered materials
    [[nodiscard]] const std::vector<std::string>& getMaterials() const { return materials; }

private:
    std::vector<CraftingRecipe> recipes;
    std::unordered_map<std::string, size_t> recipeIndex;  // recipe id -> index in recipes vector
    std::vector<std::string> materials;

    // Helper to create an item from recipe output
    std::unique_ptr<Item> createItemFromRecipe(const CraftingOutput& output) const;

    // Helper to parse material type from string
    static MaterialType parseMaterialType(const std::string& materialStr);

    // Helper to parse weapon/armor type from item id
    static WeaponType parseWeaponType(const std::string& itemId);
    static ArmourType parseArmourType(const std::string& itemId);
};

#endif //ULTIMATNIHRA_CRAFTING_H
