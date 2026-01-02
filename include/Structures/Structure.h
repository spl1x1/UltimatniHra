//
// Created by Lukáš Kaplánek on 04.12.2025.
//

#ifndef ULTIMATNIHRA_STRUCTURE_H
#define ULTIMATNIHRA_STRUCTURE_H
#include <list>
#include <memory>
#include <SDL3/SDL_rect.h>

#include "../Application/dataStructures.h"
#include "../Menu/RmlUi_Renderer_SDL.h"
#include "../Server/Server.h"


class ISprite;

enum class structureType{
    HOUSE,
    FARM,
    BARRACKS,
    TOWER,
    TREE
};

class IStructure {

public:
    virtual ~IStructure() = default;
    [[nodiscard]] virtual structureType getType() const = 0;
    [[nodiscard]] virtual int getId() const = 0;
    virtual bool wasProperlyInitialized() = 0;
    virtual void Tick(float deltaTime) = 0;
    [[nodiscard]] virtual RenderingContext GetRenderingContext() const = 0;
    [[nodiscard]] virtual HitboxContext GetHitboxContext() = 0;
};

class StructureRenderingComponent {
    Coordinates fourCorners[4];
    std::unique_ptr<ISprite> sprite;
    std::unique_ptr<SDL_FRect> Rect = std::make_unique<SDL_FRect>();

    public:

    //Methods
    [[nodiscard]] RenderingContext getRenderingContext() const;
    void Tick(float deltaTime) const;
    void SetVariant(int variant) const;

    explicit StructureRenderingComponent(std::unique_ptr<ISprite> sprite, Coordinates topLeft);

};

class StructureHitbox {
    struct TrueCoordinates {
        int x;
        int y;
    };

    Coordinates topLeftCorner;
    std::shared_ptr<Server> server;
    std::list<TrueCoordinates> hitboxPoints{};
    HitboxContext hitboxContext;

    void updateCollisionMap(int value, int checkValue = -2) const;
    [[nodiscard]] bool checkCollisionMap() const;

public:
    //Methods
    [[nodiscard]] Coordinates getTopLeftCorner() const;
    void SetTopLeftCorner(Coordinates topLeftCorner);

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
     * @param posX - X relativni pozice oproti startovniho boduf
     * @param posY - Y relativni pozice oproti startovniho bodu
     */
    void addPoint(int posX, int posY);
    /*
     * Finalizuje hitbox struktury a zapise ho do collision mapy
     */

    [[nodiscard]] HitboxContext getHitboxContext();

    [[nodiscard]] bool finalize(int id) const;
    void destroy(int id) const;

    //Constructor and Destructor
    StructureHitbox(const std::shared_ptr<Server>& server, Coordinates topLeftCorner);
    StructureHitbox(const std::shared_ptr<Server>& server ); //Empty constructor for default initialization
};


#endif //ULTIMATNIHRA_STRUCTURE_H