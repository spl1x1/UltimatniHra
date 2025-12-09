//
// Created by USER on 09.12.2025.
//

#include "../../include/Structures/Tree.h"

#include "../../include/Sprites/Sprite.hpp"
#include "../../include/Sprites/TreeSprite.hpp"

structureType Tree::getType() const {
    return structureType::TREE; // Example type, change as needed
}

int Tree::getId() const {
    return _id;
}

void Tree::render(SDL_Renderer& windowRenderer, SDL_FRect& cameraRectangle, const std::unordered_map<std::string, SDL_Texture*>& textures) const {
    _renderingComponent.renderSprite(windowRenderer, cameraRectangle, textures);
}

Tree:: Tree(int id, Coordinates topLeftCorner, const std::shared_ptr<Server> &server)
    : _id(id),
      _renderingComponent(std::make_unique<TreeSprite>(), topLeftCorner),
      _hitboxComponent(server, topLeftCorner) {
    _hitboxComponent.addPoint(1,1);
}