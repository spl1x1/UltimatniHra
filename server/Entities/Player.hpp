//
// Created by Lukáš Kaplánek on 13.11.2025.
//

#ifndef PLAYER_HPP
#define PLAYER_HPP
#include "Entity.h"
#include <SDL3/SDL.h>

enum class PlayerEvents{
    MOVE,
    ATTACK,
    PLACE,
    INTERACT,
    INVENTORY
};

struct PlayerEvent {
    PlayerEvents type;
    float data1;
    float data2;
};

class Player final : public Entity {
    bool Move(float dX, float dY) override;

    Player(int id, float maxHealth, Coordinates coordinates, Server *server ,float speed, Sprite *sprite);

public:
    SDL_FRect *cameraRect = nullptr;
    SDL_FRect *cameraWaterRect = nullptr;

    void handleEvent(PlayerEvent e); //TODO: implement

    ~Player() override;

    // Constructors

    // Initializes the player character for client, should be called only once
    static void ClientInit(Server *server);
    static Player* ClientInitLoaded(Server *server); //TODO: implement loading from save

};



#endif //PLAYER_HPP
