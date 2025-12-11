//
// Created by Lukáš Kaplánek on 01.11.2025.
//

#ifndef WATERSPRITE_H
#define WATERSPRITE_H
#include <list>
#include <memory>
#include <shared_mutex>

#include "../../include/Sprites/Sprite.hpp"

//WaterSprite class is a multiton representing water tile sprites
class WaterSprite {
    static std::list<std::unique_ptr<WaterSprite>> instances;
    static std::shared_mutex multitonMutex;
    std::shared_mutex mutexSprite;
    static float lastDeltaTime; //To make sure we are not ticking multiple times per frame

    //Sprite base data
    SpriteRenderingContext renderingContext = SpriteRenderingContext("water", Direction::OMNI,0.2f, 4, 32, 32, 0.0f, 0.0f);

    //Hidden Methods
    void tickInternal(float deltaTime);
public:
    ~WaterSprite() = default;

    //Getters
    std::tuple<std::string,SDL_FRect*> getFrame();

    //multiton access method
    static int getInstanceCount();
    static WaterSprite* getInstance(int id); //Get instance by id
    static void Tick(float deltaTime) ; //Tick all instances
    static void Init(); //Initialize multiton instances, should be called once at the start of the game
    static void Destroy(); //Destroy all instances

};



#endif //WATERSPRITE_H
