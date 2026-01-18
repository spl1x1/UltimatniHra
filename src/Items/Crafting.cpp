//
// Created by Jar Jar Banton on 15. 1. 2026.
//

#include "../../include/Items/Crafting.h"
#include "../../include/Items/inventory.h"
#include <simdjson.h>
#include <SDL3/SDL.h>
#include <algorithm>

CraftingSystem::CraftingSystem() = default;

bool CraftingSystem::loadRecipes(const std::string& filePath) {
    try {
        simdjson::ondemand::parser parser;
        const auto jsonContent = simdjson::padded_string::load(filePath);
        auto doc = parser.iterate(jsonContent);

        // Load materials list
        auto materialsArray = doc["materials"].get_array();
        for (auto material : materialsArray) {
            materials.emplace_back(std::string_view(material.get_string().value()));
        }
        SDL_Log("CraftingSystem: Loaded %zu materials", materials.size());

        // Load recipes
        auto recipesArray = doc["recipes"].get_array();
        for (auto recipeData : recipesArray) {
            auto recipeObj = recipeData.get_object();

            CraftingRecipe recipe;
            recipe.id = std::string(std::string_view(recipeObj["id"].get_string().value()));
            recipe.category = std::string(std::string_view(recipeObj["category"].get_string().value()));

            // Station is optional
            auto stationResult = recipeObj["station"].get_string();
            if (!stationResult.error()) {
                recipe.station = std::string(std::string_view(stationResult.value()));
            }

            // Parse ingredients
            auto ingredientsArray = recipeObj["ingredients"].get_array();
            for (auto ingredientData : ingredientsArray) {
                auto ingredientObj = ingredientData.get_object();
                CraftingIngredient ingredient;
                ingredient.itemId = std::string(std::string_view(ingredientObj["item"].get_string().value()));
                ingredient.amount = static_cast<int>(ingredientObj["amount"].get_int64().value());
                recipe.ingredients.push_back(ingredient);
            }

            // Parse output
            auto outputObj = recipeObj["output"].get_object();
            recipe.output.itemId = std::string(std::string_view(outputObj["item"].get_string().value()));
            recipe.output.amount = static_cast<int>(outputObj["amount"].get_int64().value());

            // Store recipe
            recipeIndex[recipe.id] = recipes.size();
            recipes.push_back(std::move(recipe));
        }

        SDL_Log("CraftingSystem: Loaded %zu recipes", recipes.size());
        return true;

    } catch (const simdjson::simdjson_error& e) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "CraftingSystem: Failed to load recipes: %s", e.what());
        return false;
    }
}

std::vector<const CraftingRecipe*> CraftingSystem::getRecipesByCategory(const std::string& category) const {
    std::vector<const CraftingRecipe*> result;
    for (const auto& recipe : recipes) {
        if (recipe.category == category) {
            result.push_back(&recipe);
        }
    }
    return result;
}

const CraftingRecipe* CraftingSystem::getRecipe(const std::string& recipeId) const {
    auto it = recipeIndex.find(recipeId);
    if (it != recipeIndex.end()) {
        return &recipes[it->second];
    }
    return nullptr;
}

bool CraftingSystem::canCraft(const CraftingRecipe& recipe, InventoryController* inventory) const {
    if (!inventory) return false;

    // Check each ingredient
    for (const auto& ingredient : recipe.ingredients) {
        int available = 0;
        MaterialType matType = parseMaterialType(ingredient.itemId);

        // Count materials in inventory
        for (int i = 0; i < 20; ++i) {  // Assuming 20 inventory slots
            Item* item = inventory->getItem(i);
            if (!item) continue;

            if (item->getType() == ItemType::MATERIAL) {
                auto* material = dynamic_cast<Material*>(item);
                if (material && material->getMaterialType() == matType) {
                    available += item->getStackSize();
                }
            }
        }

        if (available < ingredient.amount) {
            return false;
        }
    }

    return true;
}

std::unique_ptr<Item> CraftingSystem::craft(const CraftingRecipe& recipe, InventoryController* inventory) {
    if (!canCraft(recipe, inventory)) {
        SDL_Log("CraftingSystem: Cannot craft %s - missing ingredients", recipe.id.c_str());
        return nullptr;
    }

    // Consume ingredients
    for (const auto& ingredient : recipe.ingredients) {
        int toRemove = ingredient.amount;
        MaterialType matType = parseMaterialType(ingredient.itemId);

        for (int i = 0; i < 20 && toRemove > 0; ++i) {
            Item* item = inventory->getItem(i);
            if (!item) continue;

            if (item->getType() == ItemType::MATERIAL) {
                auto* material = dynamic_cast<Material*>(item);
                if (material && material->getMaterialType() == matType) {
                    int available = item->getStackSize();
                    int removeCount = std::min(available, toRemove);
                    inventory->removeItem(i, removeCount);
                    toRemove -= removeCount;
                }
            }
        }
    }

    // Create the output item
    auto craftedItem = createItemFromRecipe(recipe.output);
    SDL_Log("CraftingSystem: Crafted %s", recipe.id.c_str());

    return craftedItem;
}

std::vector<const CraftingRecipe*> CraftingSystem::getCraftableRecipes(InventoryController* inventory) const {
    std::vector<const CraftingRecipe*> result;
    for (const auto& recipe : recipes) {
        if (canCraft(recipe, inventory)) {
            result.push_back(&recipe);
        }
    }
    return result;
}

std::unique_ptr<Item> CraftingSystem::createItemFromRecipe(const CraftingOutput& output) const {
    const std::string& itemId = output.itemId;

    // Check if output is a material
    if (std::find(materials.begin(), materials.end(), itemId) != materials.end()) {
        MaterialType matType = parseMaterialType(itemId);
        auto item = ItemFactory::createMaterial(matType);
        if (output.amount > 1) {
            item->addToStack(output.amount - 1);
        }
        return item;
    }

    // Parse the item id to determine type
    // Format: material_type (e.g., "iron_sword", "leather_helmet")
    size_t underscorePos = itemId.find('_');
    if (underscorePos == std::string::npos) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "CraftingSystem: Invalid item id format: %s", itemId.c_str());
        return nullptr;
    }

    std::string materialPart = itemId.substr(0, underscorePos);
    std::string typePart = itemId.substr(underscorePos + 1);

    MaterialType material = parseMaterialType(materialPart);

    // Weapons
    if (typePart == "sword") {
        return ItemFactory::createSword(material);
    } else if (typePart == "axe") {
        return ItemFactory::createAxe(material);
    } else if (typePart == "pickaxe") {
        return ItemFactory::createPickaxe(material);
    } else if (typePart == "bow") {
        return ItemFactory::createBow(material);
    }
    // Armor
    else if (typePart == "helmet") {
        return ItemFactory::createHelmet(material);
    } else if (typePart == "chestplate") {
        return ItemFactory::createChestplate(material);
    } else if (typePart == "leggings") {
        return ItemFactory::createLeggings(material);
    } else if (typePart == "boots") {
        return ItemFactory::createBoots(material);
    }
    // Amulets
    else if (typePart == "ring") {
        return ItemFactory::createDamageAmulet(10);
    } else if (typePart == "necklace") {
        return ItemFactory::createSpeedAmulet(10);
    } else if (typePart == "crown") {
        return ItemFactory::createArmourAmulet(10);
    }

    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "CraftingSystem: Unknown item type in id: %s", itemId.c_str());
    return nullptr;
}

MaterialType CraftingSystem::parseMaterialType(const std::string& materialStr) {
    if (materialStr == "wood") return MaterialType::WOOD;
    if (materialStr == "stone") return MaterialType::STONE;
    if (materialStr == "leather") return MaterialType::LEATHER;
    if (materialStr == "iron") return MaterialType::IRON;
    if (materialStr == "steel") return MaterialType::STEEL;
    if (materialStr == "dragonscale") return MaterialType::DRAGONSCALE;
    if (materialStr == "gold") return MaterialType::GOLD;
    if (materialStr == "bronze") return MaterialType::BRONZE;
    if (materialStr == "copper") return MaterialType::COPPER;
    return MaterialType::NONE;
}

WeaponType CraftingSystem::parseWeaponType(const std::string& itemId) {
    if (itemId.find("sword") != std::string::npos) return WeaponType::SWORD;
    if (itemId.find("axe") != std::string::npos) return WeaponType::AXE;
    if (itemId.find("pickaxe") != std::string::npos) return WeaponType::PICKAXE;
    if (itemId.find("bow") != std::string::npos) return WeaponType::BOW;
    return WeaponType::SWORD;
}

ArmourType CraftingSystem::parseArmourType(const std::string& itemId) {
    if (itemId.find("helmet") != std::string::npos) return ArmourType::HELMET;
    if (itemId.find("chestplate") != std::string::npos) return ArmourType::CHESTPLATE;
    if (itemId.find("leggings") != std::string::npos) return ArmourType::LEGGINGS;
    if (itemId.find("boots") != std::string::npos) return ArmourType::BOOTS;
    return ArmourType::HELMET;
}
