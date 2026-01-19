//
// Created by Jar Jar Banton on 28. 12. 2025.
//

#include "../../include/Application/SaveGame.h"
#include "../..//include/Server/Server.h"
#include "../../include/Entities/Player.hpp"
#include <fstream>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

void SaveManager::loadAllSlots() {
    saveSlots.clear();
    saveSlots.resize(3); // 3 save sloty

    for (int i = 0; i < 3; i++) {
        saveSlots[i].slotId = i;
        std::string filepath = getSaveFilePath(i);

        if (fs::exists(filepath)) {
            deserializeSave(saveSlots[i], filepath);
        }
    }
}

SaveGame* SaveManager::getSaveSlot(int slotId) {
    if (slotId >= 0 && slotId < saveSlots.size()) {
        return &saveSlots[slotId];
    }
    return nullptr;
}

bool SaveManager::isSlotEmpty(int slotId) const {
    if (slotId >= 0 && slotId < saveSlots.size()) {
        return saveSlots[slotId].isEmpty();
    }
    return true;
}

bool SaveManager::createNewSave(int slotId, const std::string& saveName) {
    if (slotId < 0 || slotId >= saveSlots.size()) {
        return false;
    }

    SaveGame& save = saveSlots[slotId];
    save.slotId = slotId;
    save.saveName = saveName;
    save.playTime = 0.0f;
    save.lastModified = std::time(nullptr);

    // Inicializace s deafult hodnotami
    save.playerData.x = 4000.0f;
    save.playerData.y = 4000.0f;
    save.playerData.angle = 0;
    save.playerData.speed = 200.0f;
    save.playerData.health = 100.0f;
    save.playerData.maxHealth = 100.0f;

    // World data - setne se kdyz se svet vytvori
    save.worldData.seed = 0; // zmemi se s vytvorenim sveta
    save.worldData.spawnX = 4000.0f;
    save.worldData.spawnY = 4000.0f;
    save.worldData.currentLevel = 0;

    // Save to file
    std::string filepath = getSaveFilePath(slotId);
    return serializeSave(save, filepath);
}

bool SaveManager::saveGame(int slotId, Server* server) {
    if (slotId < 0 || slotId >= saveSlots.size() || !server) {
        return false;
    }

    SaveGame& save = saveSlots[slotId];
    save.lastModified = std::time(nullptr);

    // Get player
    IEntity* playerEntity = server->GetPlayer();
    if (!playerEntity) {
        return false;
    }

    // Cast to Player to access specific methods
    Player* player = dynamic_cast<Player*>(playerEntity);
    if (!player) {
        return false;
    }

    // Save player data
    Coordinates playerCoords = player->GetCoordinates();
    save.playerData.x = playerCoords.x;
    save.playerData.y = playerCoords.y;
    save.playerData.angle = player->GetAngle();
    save.playerData.speed = player->GetLogicComponent()->GetSpeed();
    save.playerData.health = player->GetHealthComponent()->GetHealth();
    save.playerData.maxHealth = player->GetHealthComponent()->GetMaxHealth();

    // Save world data
    save.worldData.seed = server->GetSeed();
    Coordinates spawnPoint = server->GetSpawnPoint();
    save.worldData.spawnX = spawnPoint.x;
    save.worldData.spawnY = spawnPoint.y;

    // TODO: implementovat play time
    // save.playTime += sessionTime; //pripravene az to budu potrebovat
    server->SaveServerState();

    std::string filepath = getSaveFilePath(slotId);
    return serializeSave(save, filepath);
}

bool SaveManager::loadGame(int slotId, Server* server) {
    if (slotId < 0 || slotId >= saveSlots.size() || !server) {
        return false;
    }

    if (saveSlots[slotId].isEmpty()) {
        return false;
    }

    currentSlot = slotId;
    SaveGame& save = saveSlots[slotId];

    // Get player
    IEntity* playerEntity = server->GetPlayer();
    if (!playerEntity) {
        return false;
    }

    auto* player = dynamic_cast<Player*>(playerEntity);
    if (!player) {
        return false;
    }

    // Load player data
    Coordinates coords = {save.playerData.x, save.playerData.y};
    player->SetCoordinates(coords);
    player->SetAngle(save.playerData.angle);
    player->SetSpeed(save.playerData.speed);
    player->GetHealthComponent()->SetHealth(save.playerData.health);
    player->GetHealthComponent()->SetMaxHealth(save.playerData.maxHealth);


    return true;
}

bool SaveManager::deleteSave(int slotId) {
    if (slotId < 0 || slotId >= saveSlots.size()) {
        return false;
    }

    std::string filepath = getSaveFilePath(slotId);

    if (fs::exists(filepath)) {
        fs::remove(filepath);
    }

    saveSlots[slotId] = SaveGame();
    saveSlots[slotId].slotId = slotId;

    std::filesystem::remove("saves/slot_" + std::to_string(slotId) +"_server_state.json");
    std::filesystem::remove("saves/worldmap_slot_" + std::to_string(slotId) +".png");

    return true;
}

std::string SaveManager::getSaveFilePath(int slotId) const {
    fs::path saveDir = "saves";
    if (!fs::exists(saveDir)) {
        fs::create_directory(saveDir);
    }

    return (saveDir / ("save_" + std::to_string(slotId) + ".sav")).string();
}

bool SaveManager::serializeSave(const SaveGame& save, const std::string& filepath) {
    std::ofstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }

    int version = 1;
    file.write(reinterpret_cast<const char*>(&version), sizeof(version));

    // Save name
    size_t nameLen = save.saveName.size();
    file.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
    file.write(save.saveName.c_str(), nameLen);

    // Metadata
    file.write(reinterpret_cast<const char*>(&save.slotId), sizeof(save.slotId));
    file.write(reinterpret_cast<const char*>(&save.playTime), sizeof(save.playTime));
    file.write(reinterpret_cast<const char*>(&save.lastModified), sizeof(save.lastModified));

    // Player data
    file.write(reinterpret_cast<const char*>(&save.playerData.x), sizeof(save.playerData.x));
    file.write(reinterpret_cast<const char*>(&save.playerData.y), sizeof(save.playerData.y));
    file.write(reinterpret_cast<const char*>(&save.playerData.angle), sizeof(save.playerData.angle));
    file.write(reinterpret_cast<const char*>(&save.playerData.speed), sizeof(save.playerData.speed));
    file.write(reinterpret_cast<const char*>(&save.playerData.health), sizeof(save.playerData.health));
    file.write(reinterpret_cast<const char*>(&save.playerData.maxHealth), sizeof(save.playerData.maxHealth));

    // World data
    file.write(reinterpret_cast<const char*>(&save.worldData.seed), sizeof(save.worldData.seed));
    file.write(reinterpret_cast<const char*>(&save.worldData.spawnX), sizeof(save.worldData.spawnX));
    file.write(reinterpret_cast<const char*>(&save.worldData.spawnY), sizeof(save.worldData.spawnY));
    file.write(reinterpret_cast<const char*>(&save.worldData.currentLevel), sizeof(save.worldData.currentLevel));

    file.close();
    return true;
}

bool SaveManager::deserializeSave(SaveGame& save, const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }

    // Version
    int version;
    file.read(reinterpret_cast<char*>(&version), sizeof(version));

    // Save name
    size_t nameLen;
    file.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
    save.saveName.resize(nameLen);
    file.read(&save.saveName[0], nameLen);

    // Metadata
    file.read(reinterpret_cast<char*>(&save.slotId), sizeof(save.slotId));
    file.read(reinterpret_cast<char*>(&save.playTime), sizeof(save.playTime));
    file.read(reinterpret_cast<char*>(&save.lastModified), sizeof(save.lastModified));

    // Player data
    file.read(reinterpret_cast<char*>(&save.playerData.x), sizeof(save.playerData.x));
    file.read(reinterpret_cast<char*>(&save.playerData.y), sizeof(save.playerData.y));
    file.read(reinterpret_cast<char*>(&save.playerData.angle), sizeof(save.playerData.angle));
    file.read(reinterpret_cast<char*>(&save.playerData.speed), sizeof(save.playerData.speed));
    file.read(reinterpret_cast<char*>(&save.playerData.health), sizeof(save.playerData.health));
    file.read(reinterpret_cast<char*>(&save.playerData.maxHealth), sizeof(save.playerData.maxHealth));

    // World data
    file.read(reinterpret_cast<char*>(&save.worldData.seed), sizeof(save.worldData.seed));
    file.read(reinterpret_cast<char*>(&save.worldData.spawnX), sizeof(save.worldData.spawnX));
    file.read(reinterpret_cast<char*>(&save.worldData.spawnY), sizeof(save.worldData.spawnY));
    file.read(reinterpret_cast<char*>(&save.worldData.currentLevel), sizeof(save.worldData.currentLevel));

    file.close();
    return true;
}