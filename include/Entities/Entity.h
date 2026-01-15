//
// Created by Lukáš Kaplánek on 25.10.2025.
//

/*
 *Vytvorit command dispatch thread pro entity, struktury atd.
 *access pres konzoli ingame
 */

#ifndef ENTITY_H
#define ENTITY_H
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>
#include <SDL3/SDL_render.h>

#include "../Server/Server.h"
#include "../Sprites/Sprite.hpp"
#include "../Entities/EntityEvent.h"

class IEntity;
class EventBindings;

class EntityRenderingComponent {
    friend class EventBindings;

    std::unique_ptr<ISprite> sprite{nullptr};
    std::unique_ptr<SDL_FRect> rect;
    Coordinates offset{-1.0f, -1.0f}; //Offset to center sprite based on its dimensions and hitbox

public:
    void Tick(float deltaTime) const;
    [[nodiscard]] Coordinates CalculateCenterOffset(IEntity& entity); //Returns offset to center sprite based on its dimensions and hitbox

    //Setters
    void PlayAnimation(AnimationType animation, Direction direction, int variant, bool ForceReset = false) const;

    //Getters
    static Direction GetDirectionBaseOnAngle(int angle) ;
    [[nodiscard]] RenderingContext GetRenderingContext() const;
    [[nodiscard]] std::tuple<float,int> GetFrameTimeAndCount() const;

    //Constructor
    explicit EntityRenderingComponent(std::unique_ptr<ISprite> sprite);
    EntityRenderingComponent(const EntityRenderingComponent&) = delete;
    EntityRenderingComponent(EntityRenderingComponent&&) = default;
};

class EntityCollisionComponent {
    friend class EventBindings;
public:
    struct HitboxData {
        Coordinates corners[4];
        bool disabledCollision{false};
        bool colliding{false};
    };
private:
    //Defines 4 hitbox corners
    HitboxData _hitbox;

public:
    //Methods

    //Check collision with structures, entities can collide with each other
    bool CheckCollision(float newX, float newY, const Server* server);
    [[nodiscard]] bool CheckCollisionAt(float newX, float newY, const Server* server) const;
    [[nodiscard]] bool CheckPoint(Coordinates coordinates, IEntity& entity) const;


    //Setters
    void SetHitbox(const HitboxData &hitbox);
    void DisableCollision(bool Switch = true);
    void SwitchCollision();

    //Getters
    [[nodiscard]] HitboxData* GetHitbox();
    [[nodiscard]] CollisionStatus GetCollisionStatus() const;
    [[nodiscard]]  HitboxContext GetHitboxContext() const;

    //Constructor
    explicit EntityCollisionComponent(const HitboxData &hitbox): _hitbox(hitbox){}
};

class EntityLogicComponent {
    friend class EventBindings;
    static constexpr float threshold = 1.0f; //Threshold to consider reached target
    Coordinates coordinates{0.0f, 0.0f};
    std::vector<std::unique_ptr<EntityEvent>> events{};
    std::vector<std::unique_ptr<EntityEvent>> queueUpEvents{}; //Events to be processed in next tick, usually result of scripts or other events
    std::set<EntityEvent::Type> interruptedEvents{};

    int angle{0};
    float speed{0};
    bool interrupted{false};
    float lockTime{0};
    float currentTime{0};

    void SetAngleBasedOnMovement(float dX, float dY); //Sets angle based on movement direction

public:
    void SetCoordinates(const Coordinates &newCoordinates);
    [[nodiscard]] Coordinates GetCoordinates() const;

    void SetAngle(int newAngle);
    [[nodiscard]] int GetAngle() const;

    void SetSpeed(float newSpeed);
    [[nodiscard]] float GetSpeed() const;

    bool IsInterrupted() const;

    //Methods
    bool Move(float deltaTime, float dX, float dY, EntityCollisionComponent &collisionComponent, const Server* server);
    void MoveTo(float deltaTime, float targetX, float targetY,IEntity* entity);
    void PerformAttack(IEntity* entity, int attackType, int damage);
    void Tick(const Server* server, IEntity &entity); //Process tasks when not already in progress else continue
    void AddEvent(std::unique_ptr<EntityEvent> eventData);

    //Constructor
    explicit EntityLogicComponent() = default;
};

class EntityHealthComponent {
    friend class EventBindings;
    int health{0};
    int maxHealth{0};
public:
    //Methods
    void TakeDamage(int damage);
    void Heal(int amount);

    //Setters
    void SetHealth(int newHealth);
    void SetMaxHealth(int newMaxHealth);

    //Getters
    [[nodiscard]] int GetHealth() const;
    [[nodiscard]] int GetMaxHealth() const;
    [[nodiscard]] bool isDead() const;

    //Constructor
    EntityHealthComponent(const int health, const int maxHealth): health(health), maxHealth(maxHealth){}
};

class EntityInventoryComponent {
    friend class EventBindings;
    //Inventory data and methods would go here
    //To be implemented
};

class EventBindings {
    static std::unique_ptr<EventBindings> instance;
    std::unordered_map<EntityEvent::Type, std::function<void(IEntity* entity, const EntityEvent* eventData)>> bindings{};

public:
    static void Callback(IEntity* entity, const EntityEvent* eventData);
    static void InitializeBindings();
};

class IEntity {
public:
    //Interface methods
    virtual void Tick() = 0;
    virtual RenderingContext GetRenderingContext() = 0;

    //Setters
    virtual void SetId(int newId) = 0;
    virtual void SetCoordinates(const Coordinates &newCoordinates) = 0;
    //Sets entity angle in degrees
    virtual void SetAngle(int newAngle) = 0;
    virtual void SetSpeed(float newSpeed) = 0;
    virtual  void SetEntityCollision(bool disable) = 0;
    //Event
    virtual void AddEvent(std::unique_ptr<EntityEvent> eventData) = 0;

    //Getters

    //Returns true entity coordinates (sprite center)
    [[nodiscard]] virtual Coordinates GetCoordinates() const = 0;
    //Returns entity center coordinates
    [[nodiscard]] virtual Coordinates GetEntityCenter() = 0;
    //Returns entity collision status
    [[nodiscard]] virtual CollisionStatus GetCollisionStatus() const = 0;
    [[nodiscard]] virtual int GetAngle() const = 0;
    [[nodiscard]] virtual HitboxContext GetHitboxRenderingContext() const = 0;
    [[nodiscard]] virtual int GetId() const = 0;
    [[nodiscard]] virtual int GetReach() const = 0;


    //Entity component getters

    //Get EntityCollisionComponent
    virtual EntityCollisionComponent* GetCollisionComponent() = 0;
    //Get EntityLogicComponent
    virtual EntityLogicComponent* GetLogicComponent() = 0;
    //Get EntityHealthComponent
    virtual EntityHealthComponent* GetHealthComponent() = 0;
    //Get EntityRenderingComponent
    virtual EntityRenderingComponent* GetRenderingComponent() = 0;
    //Get EntityInventoryComponent
    virtual EntityInventoryComponent* GetInventoryComponent() = 0;

    //Get server pointer
    [[nodiscard]] virtual Server* GetServer() const = 0;

    //Disable copy constructor and allow move constructor
    IEntity(const IEntity&) =delete;
    virtual ~IEntity() = default;
    IEntity() = default;
};

#endif //ENTITY_H
