//
// Created by Lukáš Kaplánek on 13.11.2025.
//

#include "../../include/Entities/Player.hpp"
#include "../../include/Sprites/PlayerSprite.hpp"
#include <memory>


void Player::handleEvent(PlayerEvent e) {
    switch (e.type) {
        case PlayerEvents::MOVE:
            Move(e.data1, e.data2, e.deltaTime);
            break;
        default:
            break;
    }
}

Player::Player(int id, float maxHealth, Coordinates coordinates ,const std::shared_ptr<Server>& server ,float speed): Entity(id ,maxHealth,coordinates, EntityType::PLAYER, server ,speed, std::make_unique<PlayerSprite>()) {
    Hitbox playerHitbox = {
        {
            {32, 32}, // TOP_LEFT
            {64 ,32}, // TOP_RIGHT
            {64, 65}, // BOTTOM_RIGHT
            {32,65} // BOTTOM_LEFT
        }
    };
    SetHitbox(playerHitbox);
    setSpriteOffsetX(47);
    setSpriteOffsetY(47);
};

void Player::ClientInit(const std::shared_ptr<Server>& server) {
    auto player = std::make_shared<Player>(0, 100.0f, server->getSpawnPoint(), server, 200.0f);
    server->addPlayer(player);
}
