//
// Created by Jar Jar Banton on 3. 12. 2025.
//

#ifndef ULTIMATNIHRA_ITEM_H
#define ULTIMATNIHRA_ITEM_H
#include <string>
#include <memory>

class Player;

enum class ItemType {
    WEAPON,
    ARMOUR,
    MATERIAL,
    CONSUMABLE,
    AMULET,
    PLACEABLE,
};
enum class WeaponType {
    AXE,
    SWORD,
    PICKAXE,
    BOW
};
enum class ArmourType {
    HELMET,
    CHESTPLATE,
    LEGGINGS,
    BOOTS,
};
enum class MaterialType {
    NONE,
    WOOD,
    STONE,
    LEATHER,
    IRON,
    STEEL,
    DRAGONSCALE,
    GOLD,
    BRONZE,
    COPPER
};
enum class ConsumableType {
    HEALTH_POTION,
    DAMAGE_BOOST,
    SPEED_BOOST,
};
enum class AmuletType {
    SPEED,
    DAMAGE,
    ARMOUR,
};

enum class PlaceableType {
    CHEST,
    CRAFTING_TABLE,
};

class Item {
protected:
    std::string name;
    std::string description;
    std::string iconPath;
    ItemType type;
    int value;
    bool stackable;
    int stackSize;
    int maxStackSize;
public:
    Item(std::string  name, std::string  desc, ItemType type, int value, bool stackable = false, int maxStack = 1);
    virtual ~Item() = default;
    [[nodiscard]] std::string getName() const;
    [[nodiscard]] std::string getDesription() const;
    [[nodiscard]] std::string getIconPath() const { return iconPath; }
    [[nodiscard]] ItemType getType() const {return type;};
    [[nodiscard]] int getValue() const { return value; }
    [[nodiscard]] bool isStackable() const { return stackable; }
    [[nodiscard]] int getStackSize() const { return stackSize; }
    [[nodiscard]] int getMaxStackSize() const { return maxStackSize; }

    void setIconPath(const std::string& path) { iconPath = path; }
    void addToStack(int amount);
    bool removeFromStack(int amount);

    virtual void use(Player* player) = 0;
    [[nodiscard]] virtual std::string getDisplayInfo() const =0;
};

class Weapon : public Item{
private:
    WeaponType weaponType;
    MaterialType material;
    int damage;
    float attackSpeed;
    int durability;
    int maxDurability;
public:
    Weapon(const std::string& name, WeaponType wType, MaterialType mat, int dmg, float atkSpd, int dur);
    [[nodiscard]] WeaponType getWeaponType() const { return weaponType; }
    [[nodiscard]] MaterialType getMaterial() const { return material;}
    [[nodiscard]] int getDamage() const { return damage; }
    [[nodiscard]] float getAttackSpeed() const { return attackSpeed; }
    [[nodiscard]] int getDurability() const { return durability; }
    [[nodiscard]] int getMaxDurability() const { return maxDurability; }

    void reduceDurability(int amount);
    [[nodiscard]] bool isBroken() const { return durability <= 0; }
    void use(Player* player) override;
    [[nodiscard]] std::string getDisplayInfo() const override;
};

class Armour : public Item {
private:
    ArmourType armourType;
    MaterialType material;
    int defense;
    int durability;
    int maxDurability;
public:
    Armour(const std::string& name, ArmourType aType, MaterialType mat, int def, int dur);
    [[nodiscard]] ArmourType getArmourType() const { return armourType; }
    [[nodiscard]] MaterialType getMaterial() const { return material; }
    [[nodiscard]] int getDefense() const { return defense; }
    [[nodiscard]] int getDurability () const {return durability;}
    [[nodiscard]] int getMaxDurability() const {return maxDurability;}
    void reduceDurability(int amount);
    [[nodiscard]] bool isBroken() const { return durability <= 0; }
    void use(Player* player) override;
    [[nodiscard]] std::string getDisplayInfo() const override;
};

class Consumable : public Item {
private:
    ConsumableType consumableType;
    int effectValue;
    float effectDuration;  // bude pak v seKundach

public:
    Consumable(const std::string& name, ConsumableType cType, int effect, float duration = 0.0f);
    [[nodiscard]] ConsumableType getConsumableType() const { return consumableType; }
    [[nodiscard]] int getEffectValue() const { return effectValue; }
    [[nodiscard]] float getEffectDuration() const { return effectDuration; }

    void use(Player* player) override;
    [[nodiscard]] std::string getDisplayInfo() const override;
};

class Material : public Item {
private:
    MaterialType materialType;
public:
    Material(const std::string& name, MaterialType tier);
    [[nodiscard]] MaterialType getMaterialType() const { return materialType; }
    void use(Player* player) override;
    [[nodiscard]] std::string getDisplayInfo() const override;
};

class Amulet : public Item {
private:
    AmuletType amuletType;
    int effectValue;
public:
    Amulet(const std::string& name, AmuletType aType, int effect);
    [[nodiscard]] AmuletType getAmuletType() const { return amuletType; }
    [[nodiscard]] int getEffectValue() const { return effectValue; }
    void use(Player* player) override;
    [[nodiscard]] std::string getDisplayInfo() const override;
};

class Placeable : public Item {
private:
    PlaceableType placeableType;
public:
    Placeable(const std::string& name, PlaceableType pType);
    [[nodiscard]] PlaceableType getPlaceableType() const { return placeableType; }
    void use(Player* player) override;
    [[nodiscard]] std::string getDisplayInfo() const override;
};
// Item serialization data structure
struct ItemData {
    int type;           // ItemType
    int subType;        // WeaponType, ArmourType, etc.
    int material;       // MaterialType
    int stackSize;
    int durability;
    int effectValue;
    float effectDuration;
};

namespace ItemFactory {
    // Weapons
    std::unique_ptr<Weapon> createAxe(MaterialType material);
    std::unique_ptr<Weapon> createPickaxe(MaterialType material);
    std::unique_ptr<Weapon> createSword(MaterialType material);
    std::unique_ptr<Weapon> createBow(MaterialType material);

    // Armor
    std::unique_ptr<Armour> createHelmet(MaterialType material);
    std::unique_ptr<Armour> createChestplate(MaterialType material);
    std::unique_ptr<Armour> createLeggings(MaterialType material);
    std::unique_ptr<Armour> createBoots(MaterialType material);

    // Consumables
    std::unique_ptr<Consumable> createHealthPotion(int healAmount);
    std::unique_ptr<Consumable> createDamageBoost(int damageIncrease, float duration);
    std::unique_ptr<Consumable> createSpeedBoost(int speedIncrease, float duration);

    // Materials
    std::unique_ptr<Material> createMaterial(MaterialType tier);

    // Amulets
    std::unique_ptr<Amulet> createSpeedAmulet(int speedBonus);
    std::unique_ptr<Amulet> createDamageAmulet(int damageBonus);
    std::unique_ptr<Amulet> createArmourAmulet(int armourBonus);

    // Placeables
    std::unique_ptr<Placeable> createChest();
    std::unique_ptr<Placeable> createCraftingTable();

    // Helper to get material name
    std::string getMaterialName(MaterialType tier);

    // Serialization helpers
    ItemData serializeItem(const Item* item);
    std::unique_ptr<Item> deserializeItem(const ItemData& data);
}

#endif //ULTIMATNIHRA_ITEM_H