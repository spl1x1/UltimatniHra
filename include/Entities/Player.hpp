//
// Created by Lukáš Kaplánek on 13.11.2025.
//

#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <SDL3/SDL.h>
#include "../Entities/Entity.h"


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
    float deltaTime;
};

class Player final : public Entity {
    using Entity::Move;
public:
    void handleEvent(PlayerEvent e); //TODO: implement

    // Constructors
    Player(int id, float maxHealth, Coordinates coordinates, const std::shared_ptr<Server>& server ,float speed);

    // Initializes the player character for client, should be called only once
    static void ClientInit(const std::shared_ptr<Server>& server);
    static void ClientInitLoaded(const std::shared_ptr<Server>& server); //TODO: implement loading from save

};



#endif //PLAYER_HPP
