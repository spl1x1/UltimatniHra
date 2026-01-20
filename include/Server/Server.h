//
// Created by Lukáš Kaplánek on 25.10.2025.
//

#ifndef SERVERSTRUCS_H
#define SERVERSTRUCS_H
#include <map>
#include <memory>
#include <shared_mutex>
#include <mutex>
#include <set>
#include <functional>
#include <random>
#include <SDL3/SDL_timer.h>

#include "AiManager.h"
#include "../Application/dataStructures.h"
#include "../Application/MACROS.h"
#include "../Window/WorldStructs.h"

class Item;

class Player;
enum class structureType;
class Sprite;
class WorldData;
enum class EntityType;
class IStructure;
class EntityEvent;



class Server : public std::enable_shared_from_this<Server> {
    std::shared_mutex serverMutex;

    int seed = 0;
    float deltaTime = 0.0f;

    WorldData worldData;
    Coordinates spawnPoint = {4000.0f, 4000.0f};

    //mozna by vector byl lepsi, ale toto by melo setrit pamet
    std::map<int,std::shared_ptr<class IEntity>> entities{};
    std::vector<int> entitiesToRemove{};

    std::map<int,std::shared_ptr<IStructure>> structures{};
    std::vector<int> structuresToRemove{};

    std::shared_ptr<Player> localPlayer{nullptr}; //Pointer na lokalniho hrace pro rychle

    //ID counters, k limitu se nikdy nedostaneme reclaim neni nutny
    int nextEntityId = 1; //0 je vyhrazeno pro lokalniho hrace
    int nextStructureId = 1; // 0 neni platne ID

    std::vector<int> reclaimedStructureIds{}; //Pro pripad ze budeme chtit reclaimovat
    std::vector<int> reclaimedEntityIds{};

    std::set<int> StructureIdCache; //Cache pro rychlejsi hledani entit v oblasti, set protoze nechceme duplikaty
    std::vector<DamageArea> damagePoints{}; //List souradnic kde se aplikuje damage, aby se neaplikovala vicekrat na stejne misto v jednom ticku
    std::vector<DamageArea> lastDamagePoints{}; //List souradnic kde se aplikuje damage, aby se neaplikovala vicekrat na stejne misto v jednom ticku


    struct cacheValidity {
        bool isCacheValid{false};
        int rangeForCacheUpdate{320}; //Pokud se hrac pohnul o tuto vzdalenost, tak se cache invaliduje 320 = 10 tiles
        Coordinates lastPlayerPos{0.0f, 0.0f};
    } cacheValidityData;

    int getNextEntityId();; //Vraci dalsi volne ID entity, neni thread safe, vola se jen v addEntity
    int getNextStructureId(); //Vraci dalsi volne ID struktury, neni thread safe, vola se jen v addStructure


    AiManager aiManager; //Ai automat handler
    void setupSlimeAi(IEntity* entity);

    // Callback for adding items to UI inventory
    std::function<bool(std::unique_ptr<Item>)> onItemDropped;

    struct spawnGenerator {
        std::mt19937 generator = std::mt19937(SDL_GetTicks());
        std::uniform_int_distribution<int> dist =  std::uniform_int_distribution(0,  1000);
        std::uniform_int_distribution<int> distVariant = std::uniform_int_distribution(1, 3);
    } spawnGen;

public:

    //Setters
    void SetSeed(const int newSeed) {seed = newSeed;} //Teoreticky nemusi byt thread safe callujeme jen pri inicializaci)
    void SetSpawnPoint(Coordinates newSpawnPoint);//Teoreticky nemusi byt thread safe callujeme jen pri inicializaci)
    void SetDeltaTime(float dt); //Musi byt thread safe protoze se vola kazdy frame, a k datum muze pristupovat vice threadu
    void SetEntityPos(int entityId, Coordinates newCoordinates); //Musi byt thread safe, nastavuje pozici entity podle id
    void SetEntityCollision(int entityId, bool disable); //Musi byt thread safe, nastavuje zda ma entity kolidovat
    void SetMapValue(int x, int y, WorldData::MapType mapType, int value); //Musi byt thread safe, nastavuje hodnotu collision mapy na danych souradnicich
    void SetMapValue_unprotected(int x, int y, WorldData::MapType mapType, int value); //Unprotected verze setMapValue , nastavuje hodnotu collision mapy na danych souradnicich

    //Getters
    [[nodiscard]] float GetDeltaTime(); //Musi byt thread safe protoze se vola kazdy frame, a k datum muze pristupovat vice threadu
    [[nodiscard]] float GetDeltaTime_unprotected() const;
    [[nodiscard]] Coordinates GetSpawnPoint() const {return spawnPoint;} //Nemusi byt thread safe, spawn point se nemeni po spusteni serveru
    [[nodiscard]] int GetSeed() const {return seed;} //Nemusi byt thread safe, seed se nemeni
    [[nodiscard]] int GetMapValue(int x, int y, WorldData::MapType mapType= WorldData::COLLISION_MAP); //Musi byt thread safe, vraci hodnotu collision mapy na danych souradnicich
    [[nodiscard]] int GetMapValue_unprotected(int x, int y, WorldData::MapType mapType= WorldData::COLLISION_MAP) const;
    ; //Verze bez locku, pro vnitrni pouziti
    [[nodiscard]] Coordinates GetEntityPos(int entityId); //Musi byt thread safe, vraci pozici entity podle id
    [[nodiscard]] IEntity* GetEntity(int entityId); //Metoda je thread safe, ale operace s pointerem na entitu ne
    [[nodiscard]] IEntity* GetEntity_unprotected(int entityId) const;
    [[nodiscard]] IStructure* GetStructure(int structureId); //Metoda je thread safe, ale operace s pointerem na entitu ne
    [[nodiscard]] bool IsEntityColliding(int entityId);
    [[nodiscard]] std::map<int,std::shared_ptr<IEntity>> GetEntities(); //Musi byt thread safe, vraci kopii entity mapy
    [[nodiscard]] std::map<int,std::shared_ptr<IStructure>> GetStructures(); //Musi byt thread safe, vraci kopii strukturu mapy
    [[nodiscard]] IEntity*  GetPlayer(); //Metoda je thread safe, ale operace s pointerem na entitu ne
    [[nodiscard]] IEntity* GetPlayer_unprotected() const; //Unprotected verze GetPlayer
    [[nodiscard]] std::shared_ptr<Server> GetSharedPtr();
    [[nodiscard]] std::vector<DamageArea> GetDamagePoints();
    [[nodiscard]] AiManager& GetAiManager();
    [[nodiscard]] AiManager& GetAiManager_unprotected();
    [[]] WorldData& GetWorldData();

    //Methods
    void GenerateStructures(); //Generuje stromy na mape sveta pro jistotu lockuje mutex serveru, volat při prvním vytvoreni jinak load
    void GenerateWorld(); //Generuje mapy sveta pro jistotu lockuje mutex serveru
    void Tick(); //Tick serveru, zatim tickuje sprity TODO: implementovat, nezapomenout na thread safety
    void PlayerUpdate(std::unique_ptr<EntityEvent> e, int playerId = 0); //Tick pro hrace TODO: implementovat, nezapomenout na thread safety
    std::set<int> GetStructuresInArea(Coordinates topLeft, Coordinates bootomLeft); //Vraci ID vsech entit v dane oblasti TODO: implementovat, nezapomenout na thread safety
    void ApplyDamageAt_unprotected(int damage, const std::vector<Coordinates>& positions, int entityId = -1); //Aplikuje damage vsem entitam v okoli dane pozice, non thread safe
    static int CalculateAngle(Coordinates center, Coordinates point); //Vypocita uhel mezi dvema objekty, nemusi byt thread safe
    std::vector<std::string> GetTileInfo(float x, float y);
    void InvalidateStructureCache(); //Invaliduje cache pro struktury
    void SendClickEvent(MouseButtonEvent event) const;

    IEntity* AddEntity(Coordinates coordinates, EntityType type, int variant = 1); //Prida na server entitu TODO: implementovat, nezapomenout na thread safety
    IEntity* AddEntity_unprotected(Coordinates coordinates, EntityType type, int variant = 1); //Prida na server entitu
    IEntity* AddEntity_unprotected(const std::shared_ptr<IEntity>& entity); //Prida na server entitu
    void AddEntity(const std::shared_ptr<IEntity>& entity); //Prida na server entitu, thread safe verze
    void AddLocalPlayer(const std::shared_ptr<Player>& player); //Prida na server lokalni instanci hrace

    void AddStructure(Coordinates coordinates,  structureType type, int innerType, int variant = 0); //Prida na server strukturu TODO: implementovat, nezapomenout na thread safety
    bool AddStructure_unprotected(Coordinates coordinates, structureType type, int innerType, int variant = 0); //Prida na server strukturu
    void AddStructure(std::unique_ptr<IStructure>); //Prida na server strukturu TODO: implementovat, nezapomenout na thread safety
    void AddStructure_unprotected(std::unique_ptr<IStructure>); //Prida na server strukturu TODO: implementovat, nezapomenout na thread safety

    void RemoveEntity(int entityId); //Remove entity from server TODO: implementovat, nezapomenout na thread safety
    void RemoveEntity_unprotected(int entityId);
    void RemoveStructure(int structureId); //Remove entity from server TODO: implementovat, nezapomenout na thread safety

    static StructureData GetStructureData(const IStructure* structure);
    static EntityData GetEntityData(IEntity* entity);

    void SaveServerState();
    void LoadServerState();
    void Reset();

    // Set callback for item drops (used by UI to receive items)
    void SetItemDropCallback(std::function<bool(std::unique_ptr<Item>)> callback);
    // Add item through callback (returns false if no callback set)
    bool AddItemToInventory(std::unique_ptr<Item> item) const;
};

#endif //SERVERSTRUCS_H
