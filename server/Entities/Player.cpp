//
// Created by Lukáš Kaplánek on 13.11.2025.
//

#include "Player.hpp"

#include "../../render/Sprites/PlayerSprite.hpp"

void Player::handleEvent(PlayerEvent e) {
    switch (e.type) {
        case PlayerEvents::MOVE:
            Move(e.data1, e.data2, e.deltaTime);
            break;
        default:
            break;
    }
}

Player::~Player() {
    delete sprite;
    delete cameraRect;
    delete cameraWaterRect;
}

Player::Player(int id, float maxHealth, Coordinates coordinates ,Server *server ,float speed, Sprite *sprite): Entity(id ,maxHealth,coordinates, EntityType::PLAYER, server ,speed, sprite) {
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

void Player::ClientInit(Server *server) {
    auto *player = new Player(0,100,server->getSpawnPoint(),server,200,new PlayerSprite());
    server->addEntity(player);
}
