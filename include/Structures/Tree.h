//
// Created by USER on 09.12.2025.
//

#ifndef TREE_H
#define TREE_H

#include "Structure.h"

class Tree: public IStructure {
    int _id;
    StructureRenderingComponent _renderingComponent;
    StructureHitbox _hitboxComponent;

public:
    //Interface implementation
    [[nodiscard]] structureType getType() const override;
    [[nodiscard]] int getId() const override;
    void Tick(float deltaTime) override;
    void render(SDL_Renderer& windowRenderer, SDL_FRect& cameraRectangle, std::unordered_map<std::string, SDL_Texture*>& textures) const override;

    //Constructor
    Tree(int id, Coordinates topLeftCorner, const std::shared_ptr<Server> &server);
    ~Tree() override;
};


#endif //TREE_H
