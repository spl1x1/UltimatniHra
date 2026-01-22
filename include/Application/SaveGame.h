//
// Created by Jar Jar Banton on 28. 12. 2025.
//

#ifndef ULTIMATNIHRA_SAVEGAME_H
#define ULTIMATNIHRA_SAVEGAME_H
#pragma once
#include <string>
#include <vector>
#include <ctime>
#include "../Entities/Player.hpp"

class Server;

struct SaveGame {
    int slotId;
    std::string saveName;
    time_t lastModified;
    float playTime; // v sekjundach

    // Player data
    struct PlayerData {
        float x, y; // coordinates
        int angle;
        float speed;
        float health;
        float maxHealth;
        // TODO: pridat inventory data sem
    } playerData;

    // World/Server data
    struct WorldData {
        int seed;
        float spawnX, spawnY;
        int currentLevel;
        // TODO: @Kaplanek_Lukas pridat este dulezite variables ktere jsem mozna zapomel
    } worldData;

    bool isEmpty() const { return saveName.empty(); }
};

class SaveManager {
public:
    static SaveManager& getInstance() {
        static SaveManager instance;
        return instance;
    }

    void loadAllSlots();
    SaveGame* getSaveSlot(int slotId);
    bool isSlotEmpty(int slotId) const;

    bool createNewSave(int slotId, const std::string& saveName);
    bool saveGame(int slotId, Server* server);
    bool loadGame(int slotId, Server* server);
    bool deleteSave(int slotId);

    int getCurrentSlot() const { return currentSlot; }
    void setCurrentSlot(int slot) { currentSlot = slot; }

    void updatePlayTime(float deltaTime);
    float getPlayTime(int slotId) const;

private:
    SaveManager() : currentSlot(-1) {}
    SaveManager(const SaveManager&) = delete;
    SaveManager& operator=(const SaveManager&) = delete;

    std::vector<SaveGame> saveSlots;
    int currentSlot;

    std::string getSaveFilePath(int slotId) const;
    bool serializeSave(const SaveGame& save, const std::string& filepath);
    bool deserializeSave(SaveGame& save, const std::string& filepath);
};


#endif //ULTIMATNIHRA_SAVEGAME_H