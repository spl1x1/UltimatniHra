//
// Created by Lukáš Kaplánek on 04.12.2025.
//

#ifndef ULTIMATNIHRA_STRUCTURE_H
#define ULTIMATNIHRA_STRUCTURE_H
#include <list>
#include <memory>
#include "../dataStructures.h"
#include "../Server/Server.h"


enum class structureType{
    HOUSE,
    FARM,
    BARRACKS,
    TOWER
};

class IStructure {
public:
    virtual ~IStructure() = default;
    [[nodiscard]] virtual structureType getType() const = 0;
    [[nodiscard]] virtual int getId() const = 0;
};

class StructureRenderingComponent {
    Coordinates fourCorners[4];
    std::unique_ptr<class Sprite> sprite;
    public:
    //Getters


    explicit StructureRenderingComponent(std::unique_ptr<Sprite> sprite= nullptr);
};

class StructureHitbox {
    struct TrueCoordinates {
        int x;
        int y;
    };

    Coordinates topLeftCorner;
    std::shared_ptr<Server> server;
    std::list<TrueCoordinates> hitboxPoints{};

    void updateCollisionMap(int value) const;

public:
    //Methods

    /*
        * Prida radek bodu do hitboxu struktury
        * @param posX - X relativni pozice oproti startovniho bodu
        * @param posY - Y relativni pozice oproti startovniho bodu
        * @param length - delka radku bodu
     */
    void addRowOfPoints(int posX, int posY,int length);
    /*
    * Prida sloupec bodu do hitboxu struktury
    * @param posX - X relativni pozice oproti startovniho bodu
    * @param posY - Y relativni pozice oproti startovniho bodu
    * @param length - delka sloupce bodu
    */
    void addColumnOfPoints(int posX, int posY,int length);
    /*
     * Prida bod do hitboxu struktury
     * @param posX - X relativni pozice oproti startovniho bodu
     * @param posY - Y relativni pozice oproti startovniho bodu
     */
    void addPoint(int posX, int posY);
    /*
     * Finalizuje hitbox struktury a zapise ho do collision mapy
     */
    void finalize();

    //Constructor and Destructor
    explicit StructureHitbox(const std::shared_ptr<Server>& server, Coordinates topLeftCorner);
    ~StructureHitbox();
};

class Structure{
    int id;
    structureType type;

    public:
    //Getters
    [[nodiscard]] int getId() const { return id; }
    [[nodiscard]] structureType getType() const { return type; }

    //Constructors
    Structure(int id, structureType type);
};


#endif //ULTIMATNIHRA_STRUCTURE_H