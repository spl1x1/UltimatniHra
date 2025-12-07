//
// Created by Lukáš Kaplánek on 25.10.2025.
//

#ifndef SERVERSTRUCS_H
#define SERVERSTRUCS_H
#include <list>
#include <map>
#include <shared_mutex>

#include "Structures/Structure.h"
#include "World/WorldStructs.h"

class Sprite;
struct PlayerEvent;
enum class EntityType;

//Defines a 2D coordinate
struct Coordinates {
    float x = 0;
    float y = 0;
};

class Server {
    std::shared_mutex serverMutex;

    int _seed = 0;
    float _deltaTime = 0.0f;

    WorldData _worldData;
    Coordinates _spawnPoint = {4000.0f, 4000.0f};

    //mozna by vector byl lepsi, ale toto by melo setrit pamet
    std::map<int,class Entity*> _entities{};
    std::map<int,class Structure*> _structures{};

    //ID counters, k limitu se nikdy nedostaneme reclaim neni nutny
    int _nextEntityId = 0; // 0 je vyhradeno pro playera
    int _nextStructureId = 0; // 0 zatim neni vyhrazeno

    int getNextEntityId(); //Vraci dalsi volne ID entity, neni thread safe, vola se jen v addEntity

public:

    //Setters
    void setSeed(int newSeed) {_seed = newSeed;} //Teoreticky nemusi byt thread safe callujeme jen pri inicializaci
    void setDeltaTime(float dt); //Musi byt thread safe protoze se vola kazdy frame, a k datum muze pristupovat vice threadu
    void setEntityPos(int entityId, Coordinates newCoordinates); //Musi byt thread safe, nastavuje pozici entity podle id
    void setEntityCollision(int entityId, bool disable); //Musi byt thread safe, nastavuje zda ma entity kolidovat


    //Getters
    [[nodiscard]] float getDeltaTime(); //Musi byt thread safe protoze se vola kazdy frame, a k datum muze pristupovat vice threadu
    [[nodiscard]] Coordinates getSpawnPoint() const {return _spawnPoint;} //Nemusi byt thread safe, spawn point se nemeni po spusteni serveru
    [[nodiscard]] int getSeed() const {return _seed;} //Nemusi byt thread safe, seed se nemeni
    [[nodiscard]] int getCollisionMapValue(int x, int y, WorldData::MapType mapType= WorldData::COLLISION_MAP); //Musi byt thread safe, vraci hodnotu collision mapy na danych souradnicich
    [[nodiscard]] Coordinates getEntityPos(int entityId); //Musi byt thread safe, vraci pozici entity podle id
    [[nodiscard]] Entity* getEntity(int entityId); //Metoda je thread safe, ale operace s pointerem na entitu ne
    [[nodiscard]] bool isEntityColliding(int entityId);
    [[nodiscard]] std::map<int,class Entity*> getEntities(); //Musi byt thread safe, vraci kopii entity mapy
    [[nodiscard]] std::list<Structure> getCopiesOfStructures(); //Musi byt thread safe, vraci kopii struktury mapy
    [[nodiscard]] std::shared_mutex* getMutex() { return &serverMutex;} //Vraci mutex serveru pro externi lockovani


    //Methods
    void generateWorld(); //Generuje mapy sveta pro jistotu lockuje mutex serveru
    void Tick(); //Tick serveru, zatim tickuje sprity TODO: implementovat, nezapomenout na thread safety
    void playerUpdate(PlayerEvent e); //Tick pro hrace TODO: implementovat, nezapomenout na thread safety

    void addEntity(Coordinates coordinates, EntityType type); //Prida na server entitu TODO: implementovat, nezapomenout na thread safety
    void addEntity(Entity* entity); //Prida na server entitu
    void addStructure(Coordinates coordinates,  structureType type); //Prida na server strukturu TODO: implementovat, nezapomenout na thread safety
    void addStructure(Entity* entity); //Prida na server strukturu TODO: implementovat, nezapomenout na thread safety

    void removeEntity(int entityId); //Remove entity from server TODO: implementovat, nezapomenout na thread safety
    void removeStructure(int structureId); //Remove entity from server TODO: implementovat, nezapomenout na thread safety
};

#endif //SERVERSTRUCS_H
