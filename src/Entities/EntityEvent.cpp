//
// Created by USER on 03.01.2026.
//

#include "../../include/Entities/EntityEvent.h"

bool EntityEvent::validateDeltaTime() const {
    return deltaTime > 0.0f;
}

EntityEvent::Type EntityEvent::GetType() const {
    return type;
}

void EntityEvent::SetDeltaTime(float dt) {
    if (dt < 0.0f) dt = 0.0f;
    deltaTime = dt;
}

float EntityEvent::GetDeltaTime() const {
    return deltaTime;
}

bool EntityEvent::validate() const {
    return true;
}


Event_Move::Event_Move(const float dX, const float dY) {
    this->type = Type::MOVE;
    this->dX = dX;
    this->dY = dY;
}

bool Event_Move::validate() const {
     return !(dX == 0.0f && dY == 0.0f) && validateDeltaTime();
}

std::unique_ptr<EntityEvent> Event_Move::Create(float dX, float dY) {
    return  std::make_unique<Event_Move>(dX, dY);
}

Event_MoveTo::Event_MoveTo(const float targetX, const float targetY) {
    this->type = Type::MOVE_TO;
    this->targetX = targetX;
    this->targetY = targetY;
}

bool Event_MoveTo::validate() const {
    return  !(targetX < 0.0f || targetY < 0.0f) && validateDeltaTime();
}

std::unique_ptr<EntityEvent> Event_MoveTo::Create(float targetX, float targetY) {
    return std::make_unique<Event_MoveTo>(targetX, targetY);
}

Event_ChangeCollision::Event_ChangeCollision() {
    this->type = Type::CHANGE_COLLISION;
}

std::unique_ptr<EntityEvent> Event_ChangeCollision::Create() {
    return std::make_unique<Event_ChangeCollision>();
}


Event_Interrupt::Event_Interrupt() {
    this->type = Type::INTERRUPT;
}

std::unique_ptr<EntityEvent> Event_Interrupt::Create() {
    return  std::make_unique<Event_Interrupt>();
}


Event_Damage::Event_Damage(const int amount) {
    this->type = Type::DAMAGE;
    this->amount = amount;
}

bool Event_Damage::validate() const {
    return amount > 0;
}

std::unique_ptr<EntityEvent> Event_Damage::Create(int amount) {
    return  std::make_unique<Event_Damage>(amount);
}

Event_Heal::Event_Heal(const int amount) {
    this->type = Type::HEAL;
    this->amount = amount;
}

bool Event_Heal::validate() const {
    return amount > 0;
}

std::unique_ptr<EntityEvent> Event_Heal::Create(int amount) {
    return  std::make_unique<Event_Heal>(amount);
}

Event_ClickMove::Event_ClickMove(const float targetX, const float targetY) {
    this->type = Type::CLICK_MOVE;
    this->targetX = targetX;
    this->targetY = targetY;
}

bool Event_ClickMove::validate() const {
    return !(targetX < 0.0f || targetY < 0.0f) && validateDeltaTime();
}

std::unique_ptr<EntityEvent> Event_ClickMove::Create(float targetX, float targetY) {
    return std::make_unique<Event_ClickMove>(targetX, targetY);
}

bool Event_InterruptSpecific::validate() const {
    return eventToInterrupt != Type::NONE;
}

std::unique_ptr<EntityEvent> Event_InterruptSpecific::Create(Type eventType) {
    return std::make_unique<Event_InterruptSpecific>(eventType);
}


Event_InterruptSpecific::Event_InterruptSpecific(const Type eventType) {
    this->eventToInterrupt = eventType;
    this->type = Type::INTERRUPT_SPECIFIC;
}

bool Event_Attack::validate() const {
    if (damage<0) return false;
    return validateDeltaTime();
}

Event_Attack::Event_Attack(const int attackType , const int damage) {
    this->type = Type::ATTACK;
    this->damage = damage;
    this->attackType = attackType;
}

std::unique_ptr<EntityEvent> Event_Attack::Create(int attackType ,int damage) {
    return std::make_unique<Event_Attack>(attackType, damage);
}


bool Event_ClickAttack::validate() const {
    if (damage<0) return false;
    if (coordinates.x <0 || coordinates.y <0) return false;
    return validateDeltaTime();
}

Event_ClickAttack::Event_ClickAttack(const int attackType , const int damage, Coordinates coordinates) {
    this->type = Type::CLICK_ATTACK;
    this->damage = damage;
    this->attackType = attackType;
    this->tile = coordinates;
}

std::unique_ptr<EntityEvent> Event_ClickAttack::Create(int attackType ,int damage, Coordinates coordinates) {
    return std::make_unique<Event_ClickAttack>(attackType, damage, coordinates);
}