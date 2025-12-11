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
    virtual void Tick(float deltaTime) = 0;
    virtual void render(SDL_Renderer& windowRenderer, SDL_FRect& cameraRectangle, std::unordered_map<std::string, SDL_Texture*>& textures) const = 0;
};

class StructureRenderingComponent {
    Coordinates fourCorners[4];
    std::unique_ptr<ISprite> sprite;
    std::unique_ptr<SDL_FRect> Rect = std::make_unique<SDL_FRect>();


    [[nodiscard]] bool dismisCorners(const SDL_FRect& windowRectangle) const;
    public:

    //Methods
    void renderSprite(SDL_Renderer& windowRenderer, SDL_FRect& cameraRectangle, std::unordered_map<std::string, SDL_Texture*>& textures) const;
    void Tick(float deltaTime) const;

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
     * @param posX - X relativni pozice oproti startovniho boduf
     * @param posY - Y relativni pozice oproti startovniho bodu
     */
    void addPoint(int posX, int posY);
    /*
     * Finalizuje hitbox struktury a zapise ho do collision mapy
     */
    void finalize() const;

    //Constructor and Destructor
    explicit StructureHitbox(const std::shared_ptr<Server>& server, Coordinates topLeftCorner);
    ~StructureHitbox();
};


#endif //ULTIMATNIHRA_STRUCTURE_H