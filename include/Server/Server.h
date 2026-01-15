//
// Created by Lukáš Kaplánek on 25.10.2025.
//

#ifndef SERVERSTRUCS_H
#define SERVERSTRUCS_H
#include <map>
#include <memory>
#include <shared_mutex>
#include <set>

#include "../Application/dataStructures.h"
#include "../Window/WorldStructs.h"

class Player;
enum class structureType;
class Sprite;
class WorldData;
enum class EntityType;
class IStructure;
class EntityEvent;


class Server : public std::enable_shared_from_this<Server> {
    std::shared_mutex serverMutex;

    int _seed = 0;
    float _deltaTime = 0.0f;

    WorldData _worldData;
    Coordinates _spawnPoint = {4000.0f, 4000.0f};

    //mozna by vector byl lepsi, ale toto by melo setrit pamet
    std::map<int,std::shared_ptr<class IEntity>> _entities{};
    std::map<int,std::shared_ptr<IStructure>> _structures{};
    std::shared_ptr<Player> localPlayer{nullptr}; //Pointer na lokalniho hrace pro rychle

    //ID counters, k limitu se nikdy nedostaneme reclaim neni nutny
    int _nextEntityId = 1; //0 je vyhrazeno pro lokalniho hrace

    int _nextStructureId = 0; // 0 zatim neni vyhrazeno

    std::vector<int> reclaimedStructureIds{}; //Pro pripad ze budeme chtit reclaimovat
    std::vector<int> reclaimedEntityIds{};

    std::set<int> StructureIdCache; //Cache pro rychlejsi hledani entit v oblasti, set protoze nechceme duplikaty
    std::vector<DamageArea> damageTiles{}; //List souradnic kde se aplikuje damage, aby se neaplikovala vicekrat na stejne misto v jednom ticku

    struct cacheValidity {
        bool isCacheValid{false};
        int rangeForCacheUpdate{320}; //Pokud se hrac pohnul o tuto vzdalenost, tak se cache invaliduje 320 = 10 tiles
        Coordinates lastPlayerPos{0.0f, 0.0f};
    } cacheValidityData;

    int getNextEntityId();; //Vraci dalsi volne ID entity, neni thread safe, vola se jen v addEntity
    int getNextStructureId(); //Vraci dalsi volne ID struktury, neni thread safe, vola se jen v addStructure

public:

    //Setters
    void setSeed(int newSeed) {_seed = newSeed;} //Teoreticky nemusi byt thread safe callujeme jen pri inicializaci
    void setDeltaTime(float dt); //Musi byt thread safe protoze se vola kazdy frame, a k datum muze pristupovat vice threadu
    void setEntityPos(int entityId, Coordinates newCoordinates); //Musi byt thread safe, nastavuje pozici entity podle id
    void setEntityCollision(int entityId, bool disable); //Musi byt thread safe, nastavuje zda ma entity kolidovat
    void setMapValue(int x, int y, WorldData::MapType mapType, int value); //Musi byt thread safe, nastavuje hodnotu collision mapy na danych souradnicich
    void setMapValue_unprotected(int x, int y, WorldData::MapType mapType, int value) const; //Unprotected verze setMapValue , nastavuje hodnotu collision mapy na danych souradnicich

    //Getters
    [[nodiscard]] float getDeltaTime(); //Musi byt thread safe protoze se vola kazdy frame, a k datum muze pristupovat vice threadu
    [[nodiscard]] float getDeltaTime_unprotected() const;
    [[nodiscard]] Coordinates getSpawnPoint() const {return _spawnPoint;} //Nemusi byt thread safe, spawn point se nemeni po spusteni serveru
    [[nodiscard]] int getSeed() const {return _seed;} //Nemusi byt thread safe, seed se nemeni
    [[nodiscard]] int getMapValue(int x, int y, WorldData::MapType mapType= WorldData::COLLISION_MAP); //Musi byt thread safe, vraci hodnotu collision mapy na danych souradnicich
    [[nodiscard]] int getMapValue_unprotected(int x, int y, WorldData::MapType mapType= WorldData::COLLISION_MAP) const {return _worldData.getMapValue(x,y, mapType);}; //Verze bez locku, pro vnitrni pouziti
    [[nodiscard]] Coordinates getEntityPos(int entityId); //Musi byt thread safe, vraci pozici entity podle id
    [[nodiscard]] IEntity* getEntity(int entityId); //Metoda je thread safe, ale operace s pointerem na entitu ne
    [[nodiscard]] IStructure* getStructure(int structureId); //Metoda je thread safe, ale operace s pointerem na entitu ne
    [[nodiscard]] bool isEntityColliding(int entityId);
    [[nodiscard]] std::map<int,std::shared_ptr<IEntity>> getEntities(); //Musi byt thread safe, vraci kopii entity mapy
    [[nodiscard]] std::map<int,std::shared_ptr<IStructure>> getStructures(); //Musi byt thread safe, vraci kopii strukturu mapy
    [[nodiscard]] IEntity*  getPlayer(); //Metoda je thread safe, ale operace s pointerem na entitu ne
    [[nodiscard]] std::shared_ptr<Server> getSharedPtr();

    //Methods
    void generateStructures(); //Generuje stromy na mape sveta pro jistotu lockuje mutex serveru, volat při prvním vytvoreni jinak load
    void generateWorld(); //Generuje mapy sveta pro jistotu lockuje mutex serveru
    void Tick(); //Tick serveru, zatim tickuje sprity TODO: implementovat, nezapomenout na thread safety
    void playerUpdate(std::unique_ptr<EntityEvent> e, int playerId = 0); //Tick pro hrace TODO: implementovat, nezapomenout na thread safety
    std::set<int> getStructuresInArea(Coordinates topLeft, Coordinates bootomLeft); //Vraci ID vsech entit v dane oblasti TODO: implementovat, nezapomenout na thread safety
    void applyDamageAt_unprotected(int damage, Coordinates position, int entityId = -1); //Aplikuje damage vsem entitam v okoli dane pozice TODO: implementovat, nezapomenout na thread safety


    void addEntity(Coordinates coordinates, EntityType type); //Prida na server entitu TODO: implementovat, nezapomenout na thread safety
    void addEntity(const std::shared_ptr<IEntity>& entity); //Prida na server entitu
    void addLocalPlayer(const std::shared_ptr<Player>& player); //Prida na server lokalni instanci hrace

    void addStructure(Coordinates coordinates,  structureType type, int innerType, int variant = 0); //Prida na server strukturu TODO: implementovat, nezapomenout na thread safety
    bool addStructure_unprotected(Coordinates coordinates, structureType type, int innerType, int variant = 0); //Prida na server strukturu
    void addStructure(std::unique_ptr<IStructure>); //Prida na server strukturu TODO: implementovat, nezapomenout na thread safety
    void addStructure_unprotected(std::unique_ptr<IStructure>); //Prida na server strukturu TODO: implementovat, nezapomenout na thread safety

    void removeEntity(int entityId); //Remove entity from server TODO: implementovat, nezapomenout na thread safety
    void removeStructure(int structureId); //Remove entity from server TODO: implementovat, nezapomenout na thread safety
};

#endif //SERVERSTRUCS_H
