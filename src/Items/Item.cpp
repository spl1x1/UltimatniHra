//
// Created by Jar Jar Banton on 3. 12. 2025.
//


#include "../../include/Items/Item.h"
#include <sstream>
#include <utility>


Item::Item(std::string  name, std::string  desc, ItemType type, int value, bool stackable, int maxStack)
    : name(std::move(name)), description(std::move(desc)), type(type), value(value), stackable(stackable), stackSize(1), maxStackSize(maxStack) {}

std::string Item::getName() const {
    return name;
}

std::string Item::getDesription() const {
    return description;
}

void Item::addToStack(int amount) {
    if (stackable) {
        stackSize = std::min(stackSize + amount, maxStackSize);
    }
}

bool Item::removeFromStack(int amount) {
    if (stackSize >= amount) {
        stackSize -= amount;
        return true;
    }
    return false;
}

std::string Item::getDisplayInfo() const {
    std::ostringstream oss;
    oss << name;
    if (stackable && stackSize > 1) {
        oss << " x" << stackSize;
    }
    return oss.str();
}

Weapon::Weapon(const std::string& name, WeaponType wType, MaterialType mat, int dmg, float atkSpd, int dur)
    : Item(name, "A weapon for combat", ItemType::WEAPON, 0, false, 1),
      weaponType(wType), material(mat), damage(dmg), attackSpeed(atkSpd), durability(dur), maxDurability(dur) {

    value = (damage * 10) + static_cast<int>(attackSpeed * 50) + (durability / 2);
}

void Weapon::reduceDurability(int amount) {
    durability = std::max(0, durability - amount);
}

void Weapon::use(Player* player) {
// az bude player classa tak equip
}

std::string Weapon::getDisplayInfo() const {
    std::ostringstream oss;
    oss << name << "\n";
    oss << "Damage: " << damage << "\n";
    oss << "Attack Speed: " << attackSpeed << "\n";
    oss << "Durability: " << durability << "/" << maxDurability;
    return oss.str();
}

Armour::Armour(const std::string& name, ArmourType aType, MaterialType mat, int def, int dur)
    : Item(name, "Protective armor", ItemType::ARMOUR, 0, false, 1),
      armourType(aType), material(mat), defense(def), durability(dur), maxDurability(dur) {

    // Calculate value based on stats
    value = (defense * 15) + (durability / 2);
}

void Armour::reduceDurability(int amount) {
    durability = std::max(0, durability - amount);
}

void Armour::use(Player* player) {
    // az bude player classa tak equip
}

std::string Armour::getDisplayInfo() const {
    std::ostringstream oss;
    oss << name << "\n";
    oss << "Defense: " << defense << "\n";
    oss << "Durability: " << durability << "/" << maxDurability;
    return oss.str();
}

Consumable::Consumable(const std::string& name, ConsumableType cType, int effect, float duration)
    : Item(name, "A consumable item", ItemType::CONSUMABLE, 0, true, 99),
      consumableType(cType), effectValue(effect), effectDuration(duration) {

    value = effectValue * 5;
}

void Consumable::use(Player* player) {
    // az bude player classa tak pouzit effect
    if (stackSize > 0) {
        stackSize--;
    }
}

std::string Consumable::getDisplayInfo() const {
    std::ostringstream oss;
    oss << name << " x" << stackSize << "\n";

    switch (consumableType) {
        case ConsumableType::HEALTH_POTION:
            oss << "Restores " << effectValue << " HP";
            break;
        case ConsumableType::DAMAGE_BOOST:
            oss << "+" << effectValue << " damage for " << effectDuration << "s";
            break;
        case ConsumableType::SPEED_BOOST:
            oss << "+" << effectValue << "% speed for " << effectDuration << "s";
            break;
    }

    return oss.str();
}

Material::Material(const std::string& name, MaterialType tier)
    : Item(name, "Crafting material", ItemType::MATERIAL, 0, true, 99), materialType(tier) {

    switch (tier) {
        case MaterialType::STONE: value = 1; break;
        case MaterialType::LEATHER: value = 5; break;
        case MaterialType::IRON: value = 10; break;
        case MaterialType::STEEL: value = 25; break;
        case MaterialType::DRAGONSCALE: value = 100; break;
        default: value = 1; break;
    }
}

void Material::use(Player* player) {
    // materialy budeme asi v craftit takze kdyz tak vymazat
}

std::string Material::getDisplayInfo() const {
    std::ostringstream oss;
    oss << name << " x" << stackSize;
    return oss.str();
}

namespace ItemFactory {

    std::string getMaterialName(const MaterialType tier) {
        switch (tier) {
            case MaterialType::WOOD: return "Wooden";
            case MaterialType::STONE: return "Stone";
            case MaterialType::LEATHER: return "Leather";
            case MaterialType::IRON: return "Iron";
            case MaterialType::STEEL: return "Steel";
            case MaterialType::DRAGONSCALE: return "Dragon Scale";
            default: return "Unknown";
        }
    }

    std::string getMaterialPrefix(const MaterialType tier) {
        switch (tier) {
            case MaterialType::WOOD: return "wood";
            case MaterialType::STONE: return "stone";
            case MaterialType::LEATHER: return "leather";
            case MaterialType::IRON: return "iron";
            case MaterialType::STEEL: return "steel";
            case MaterialType::DRAGONSCALE: return "dragonscale";
            default: return "unknown";
        }
    }

    const std::string ITEMS_ICON_BASE = "assets/textures/items/";

    std::string getWeaponIconPath(const std::string& weaponType, MaterialType material) {
        return ITEMS_ICON_BASE + getMaterialPrefix(material) + "_" + weaponType + ".png";
    }

    std::string getArmourIconPath(const std::string& armourType, MaterialType material) {
        return ITEMS_ICON_BASE + getMaterialPrefix(material) + "_" + armourType + ".png";
    }

    std::string getConsumableIconPath(const std::string& consumableType) {
        return ITEMS_ICON_BASE + consumableType + ".png";
    }

    std::string getMaterialIconPath(MaterialType material) {
        return ITEMS_ICON_BASE + getMaterialPrefix(material) + ".png";
    }

    std::unique_ptr<Weapon> createAxe(MaterialType material) {
        const std::string matName = getMaterialName(material);
        int baseDmg = 0, dur = 0;
        float atkSpd = 1.0f;

        switch (material) {
            case MaterialType::WOOD: baseDmg = 5; dur = 50; atkSpd = 0.8f; break;
            case MaterialType::STONE: baseDmg = 8; dur = 80; atkSpd = 0.9f; break;
            case MaterialType::IRON: baseDmg = 12; dur = 150; atkSpd = 1.0f; break;
            case MaterialType::STEEL: baseDmg = 18; dur = 250; atkSpd = 1.1f; break;
            default: baseDmg = 5; dur = 50; atkSpd = 0.8f; break;
        }

        auto weapon = std::make_unique<Weapon>(matName + " Axe", WeaponType::AXE, material, baseDmg, atkSpd, dur);
        weapon->setIconPath(getWeaponIconPath("axe", material));
        return weapon;
    }

    std::unique_ptr<Weapon> createPickaxe(MaterialType material) {
        const std::string matName = getMaterialName(material);
        int baseDmg = 0, dur = 0;
        float atkSpd = 0.8f;

        switch (material) {
            case MaterialType::WOOD: baseDmg = 4; dur = 60; atkSpd = 0.7f; break;
            case MaterialType::STONE: baseDmg = 6; dur = 100; atkSpd = 0.8f; break;
            case MaterialType::IRON: baseDmg = 10; dur = 200; atkSpd = 0.9f; break;
            case MaterialType::STEEL: baseDmg = 15; dur = 350; atkSpd = 1.0f; break;
            default: baseDmg = 4; dur = 60; atkSpd = 0.7f; break;
        }

        auto weapon = std::make_unique<Weapon>(matName + " Pickaxe", WeaponType::PICKAXE, material, baseDmg, atkSpd, dur);
        weapon->setIconPath(getWeaponIconPath("pickaxe", material));
        return weapon;
    }

    std::unique_ptr<Weapon> createSword(MaterialType material) {
        const std::string matName = getMaterialName(material);
        int baseDmg = 0, dur = 0;
        float atkSpd = 1.5f;

        switch (material) {
            case MaterialType::WOOD: baseDmg = 6; dur = 50; atkSpd = 1.2f; break;
            case MaterialType::STONE: baseDmg = 10; dur = 90; atkSpd = 1.3f; break;
            case MaterialType::IRON: baseDmg = 15; dur = 180; atkSpd = 1.5f; break;
            case MaterialType::STEEL: baseDmg = 22; dur = 300; atkSpd = 1.7f; break;
        case MaterialType::DRAGONSCALE: baseDmg = 30; dur = 500; atkSpd = 1.8f; break;
            default: baseDmg = 6; dur = 50; atkSpd = 1.2f; break;
        }

        auto weapon = std::make_unique<Weapon>(matName + " Sword", WeaponType::SWORD, material, baseDmg, atkSpd, dur);
        weapon->setIconPath(getWeaponIconPath("sword", material));
        return weapon;
    }

    std::unique_ptr<Weapon> createBow(MaterialType material) {
        std::string matName = getMaterialName(material);
        int baseDmg = 0, dur = 0;
        float atkSpd = 2.0f;

        switch (material) {
            case MaterialType::WOOD: baseDmg = 8; dur = 40; atkSpd = 1.5f; break;
            case MaterialType::STONE: baseDmg = 12; dur = 70; atkSpd = 1.8f; break;
            case MaterialType::IRON: baseDmg = 18; dur = 140; atkSpd = 2.0f; break;
            case MaterialType::STEEL: baseDmg = 25; dur = 220; atkSpd = 2.2f; break;
            default: baseDmg = 8; dur = 40; atkSpd = 1.5f; break;
        }

        auto weapon = std::make_unique<Weapon>(matName + " Bow", WeaponType::BOW, material, baseDmg, atkSpd, dur);
        weapon->setIconPath(getWeaponIconPath("bow", material));
        return weapon;
    }

    // Armour creation
    std::unique_ptr<Armour> createHelmet(MaterialType material) {
        std::string matName = getMaterialName(material);
        int def = 0, dur = 0;

        switch (material) {
            case MaterialType::LEATHER: def = 3; dur = 100; break;
            case MaterialType::IRON: def = 5; dur = 180; break;
            case MaterialType::STEEL: def = 8; dur = 300; break;
            case MaterialType::DRAGONSCALE: def = 15; dur = 500; break;
            default: def = 2; dur = 60; break;
        }

        auto armour = std::make_unique<Armour>(matName + " Helmet", ArmourType::HELMET, material, def, dur);
        armour->setIconPath(getArmourIconPath("helmet", material));
        return armour;
    }

    std::unique_ptr<Armour> createChestplate(MaterialType material) {
        std::string matName = getMaterialName(material);
        int def = 0, dur = 0;

        switch (material) {
            case MaterialType::LEATHER: def = 5; dur = 120; break;
            case MaterialType::IRON: def = 10; dur = 220; break;
            case MaterialType::STEEL: def = 16; dur = 380; break;
            case MaterialType::DRAGONSCALE: def = 25; dur = 650; break;
            default: def = 3; dur = 80; break;
        }

        auto armour = std::make_unique<Armour>(matName + " Chestplate", ArmourType::CHESTPLATE, material, def, dur);
        armour->setIconPath(getArmourIconPath("chestplate", material));
        return armour;
    }

    std::unique_ptr<Armour> createLeggings(MaterialType material) {
        std::string matName = getMaterialName(material);
        int def = 0, dur = 0;

        switch (material) {
            case MaterialType::LEATHER: def = 4; dur = 110; break;
            case MaterialType::IRON: def = 8; dur = 200; break;
            case MaterialType::STEEL: def = 13; dur = 340; break;
            case MaterialType::DRAGONSCALE: def = 20; dur = 600; break;
            default: def = 2; dur = 70; break;
        }

        auto armour = std::make_unique<Armour>(matName + " Leggings", ArmourType::LEGGINGS, material, def, dur);
        armour->setIconPath(getArmourIconPath("leggings", material));
        return armour;
    }

    std::unique_ptr<Armour> createBoots(MaterialType material) {
        std::string matName = getMaterialName(material);
        int def = 0, dur = 0;

        switch (material) {
            case MaterialType::LEATHER: def = 2; dur = 90; break;
            case MaterialType::IRON: def = 4; dur = 160; break;
            case MaterialType::STEEL: def = 7; dur = 280; break;
            case MaterialType::DRAGONSCALE: def = 12; dur = 480; break;
            default: def = 1; dur = 50; break;
        }

        auto armour = std::make_unique<Armour>(matName + " Boots", ArmourType::BOOTS, material, def, dur);
        armour->setIconPath(getArmourIconPath("boots", material));
        return armour;
    }

    // Consumables
    std::unique_ptr<Consumable> createHealthPotion(int healAmount) {
        std::string name;
        std::string iconName;
        if (healAmount <= 30) {
            name = "Minor Health Potion";
            iconName = "health_potion_minor";
        } else if (healAmount <= 60) {
            name = "Health Potion";
            iconName = "health_potion";
        } else {
            name = "Greater Health Potion";
            iconName = "health_potion_greater";
        }

        auto consumable = std::make_unique<Consumable>(name, ConsumableType::HEALTH_POTION, healAmount, 0.0f);
        consumable->setIconPath(getConsumableIconPath(iconName));
        return consumable;
    }

    std::unique_ptr<Consumable> createDamageBoost(int damageIncrease, float duration) {
        auto consumable = std::make_unique<Consumable>("Strength Potion", ConsumableType::DAMAGE_BOOST, damageIncrease, duration);
        consumable->setIconPath(getConsumableIconPath("strength_potion"));
        return consumable;
    }

    std::unique_ptr<Consumable> createSpeedBoost(int speedIncrease, float duration) {
        auto consumable = std::make_unique<Consumable>("Swiftness Potion", ConsumableType::SPEED_BOOST, speedIncrease, duration);
        consumable->setIconPath(getConsumableIconPath("swiftness_potion"));
        return consumable;
    }

    // Materials
    std::unique_ptr<Material> createMaterial(MaterialType tier) {
        std::string name = getMaterialName(tier);
        auto material = std::make_unique<Material>(name, tier);
        material->setIconPath(getMaterialIconPath(tier));
        return material;
    }

}