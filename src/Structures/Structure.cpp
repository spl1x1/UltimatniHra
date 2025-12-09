//
// Created by Lukáš Kaplánek on 04.12.2025.
//

#include "../../include/Structures/Structure.h"

#include "../../include/MACROS.h"
#include "../../include/Sprites/Sprite.hpp"

//StructureRenderingComponent methods

StructureRenderingComponent::StructureRenderingComponent(std::unique_ptr<Sprite> sprite, Coordinates topLeft) : sprite(std::move(sprite)){
    auto width = static_cast<float>(sprite->getWidth());
    auto height = static_cast<float>(sprite->getHeight());
    this->fourCorners[0] = topLeft; //Top-left
    this->fourCorners[1] = {topLeft.x + width, topLeft.y}; //Top-right
    this->fourCorners[2] = {topLeft.x, topLeft.y + height}; //Bottom-left
    this->fourCorners[3] = {topLeft.x + width, topLeft.y + height}; //Bottom-right
}

bool StructureRenderingComponent::dismisCorners(SDL_FRect& cameraRectangle) const {
    for (const Coordinates& corner : fourCorners) {
        if (corner.x >= cameraRectangle.x &&
            corner.x <= cameraRectangle.x + GAMERESW &&
            corner.y >= cameraRectangle.y &&
            corner.y <= cameraRectangle.y + GAMERESH
        )  return false;
    }
    return true;
}

void StructureRenderingComponent::renderSprite(SDL_Renderer& windowRenderer, SDL_FRect& cameraRectangle, const std::unordered_map<std::string, SDL_Texture*>& textures) const {
    if (dismisCorners(cameraRectangle)) return;

    auto renderingContex = sprite->getFrame();
    std::unique_ptr<SDL_FRect> spritePosition = std::make_unique<SDL_FRect>();

    spritePosition->x = fourCorners[0].x - cameraRectangle.x;
    spritePosition->y = fourCorners[0].y - cameraRectangle.y;
    spritePosition->w = static_cast<float>(sprite->getWidth());
    spritePosition->h = static_cast<float>(sprite->getHeight());

    SDL_RenderTexture(&windowRenderer, textures.at(std::get<0>(renderingContex)),std::get<1>(renderingContex).get(), spritePosition.get());
}

//StructueHitbox methods
StructureHitbox::StructureHitbox(const std::shared_ptr<Server> &server, Coordinates topLeftCorner) {
    this->topLeftCorner = topLeftCorner;
    this->server = server;
}

StructureHitbox::~StructureHitbox() {
    updateCollisionMap(0);
}

void StructureHitbox::updateCollisionMap(int value) const {
    for (const TrueCoordinates& point : hitboxPoints) {
        server->setMapValue_unprotected(point.x, point.y, WorldData::COLLISION_MAP, value);
    }
}

void StructureHitbox::addRowOfPoints(int posX, int posY, int length) {
    for (int i = 0; i < length; i++) {
        addPoint(posX + i, posY);
    }
}

void StructureHitbox::addColumnOfPoints(int posX, int posY, int length) {
    for (int i = 0; i < length; i++) {
        addPoint(posX, posY + i);
    }
}

void StructureHitbox::addPoint(int posX, int posY){
    TrueCoordinates point{};
    point.x = static_cast<int>(topLeftCorner.x)/32 + posX;
    point.y = static_cast<int>(topLeftCorner.y)/32 + posY;
    hitboxPoints.push_back(point);
}

void StructureHitbox::finalize() const {
    updateCollisionMap(1);
}
