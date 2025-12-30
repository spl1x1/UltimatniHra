//
// Created by USER on 09.12.2025.
//

#include "../../include/Structures/Tree.h"

#include <random>

#include "../../include/Application/MACROS.h"
#include "../../include/Sprites/Sprite.hpp"
#include "../../include/Sprites/TreeSprite.hpp"

structureType Tree::getType() const {
    return structureType::TREE; // Example type, change as needed
}

int Tree::getId() const {
    return _id;
}

bool Tree::wasProperlyInitialized() {
    return initialized;
}

void Tree::Tick(float deltaTime) {
    _renderingComponent.Tick(deltaTime);
}

void Tree::Render(SDL_Renderer& windowRenderer, SDL_FRect& cameraRectangle, std::unordered_map<std::string, SDL_Texture*>& textures) const {
    _renderingComponent.renderSprite(windowRenderer, cameraRectangle, textures);
}


Tree::Tree(int id, Coordinates topLeftCorner, const std::shared_ptr<Server> &server)
    : _id(id),
      _renderingComponent(std::make_unique<TreeSprite>(), topLeftCorner),
      _hitboxComponent(server, topLeftCorner) {


    _hitboxComponent.addPoint(1,1);
    initialized = _hitboxComponent.finalize(id);
}

Tree::~Tree() {
    if (!initialized) return;
    _hitboxComponent.destroy(_id);
};