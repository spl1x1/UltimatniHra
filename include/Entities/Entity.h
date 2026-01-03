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

class IEntity;

enum class Event{
    // data = float dX, float dY
    MOVE,
    // data = float targetX, float targetY
    MOVE_TO,
    // data = attackType
    ATTACK,
    // data = int structureType, Coordinates position
    PLACE,
    // data = Coordinates position
    INTERACT,
    // data = std::vector<Item> items
    INVENTORY,
    // data = float amount
    DAMAGE,
    // data = float amount
    HEAL,
    CHANGE_COLLISION,
    //causes events after to discard
    INTERRUPT,
    // data = float x, float y, player specific event
    CLICK_MOVE
};

struct EventData {
    Event type{};
    //Delta time for task processing
    float dt{};
    /*
     Parameters for the event
     */
    union {
        struct { float x{0}, y{0}; } coordinates; // For event that need coordinates
        struct {int id{0}, variant{0}, type{0}; } idRelated; // For structure, entities related events, usually interact events
        int amount{0}; // For damage/heal events
    } data;
};

class EntityRenderingComponent {
    friend class EntityScripts;
    std::unique_ptr<ISprite> _sprite{nullptr};
    std::unique_ptr<SDL_FRect> _rect;

public:
    void Tick(float deltaTime) const;

    //Setters
    void SetDirectionBaseOnAngle(int angle) const;
    void SetAnimation(AnimationType animation) const;
    void SetSprite(std::unique_ptr<ISprite> sprite);

    //Getters
    [[nodiscard]] RenderingContext GetRenderingContext() const;


    //Constructor
    explicit EntityRenderingComponent(std::unique_ptr<ISprite> sprite);
    EntityRenderingComponent(const EntityRenderingComponent&) = delete;
    EntityRenderingComponent(EntityRenderingComponent&&) = default;
};

class EntityCollisionComponent {
    friend class EntityScripts;
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
    friend class EntityScripts;
public:

    struct ScriptData {
        std::string scriptName{};
        std::function<void(IEntity&, EventData&)> function{};
    };

private:
    static constexpr float threshold = 1.0f; //Threshold to consider reached target
    Coordinates coordinates{0.0f, 0.0f};
    std::vector<EventData> events{};
    std::vector<EventData> queueUpEvents{}; //Events to be processed in next tick, usually result of scripts or other events

    std::unordered_map<std::string,ScriptData> scriptBindings; //Scripts bound to entity

    int angle{0};
    float speed{0};
    bool interrupted{false};

    void HandleEvent(const Server* server, IEntity &entity, int eventIndex);
    void SetAngleBasedOnMovement(float dX, float dY); //Sets angle based on movement direction

public:

    //Script binding methods
    void BindScript(const std::string &scriptName, const ScriptData &scriptData);
    void UnbindScript(const std::string &scriptName);
    [[nodiscard]] ScriptData GetBoundScript(const std::string &scriptName) const;

    void SetCoordinates(const Coordinates &newCoordinates);
    [[nodiscard]] Coordinates GetCoordinates() const;

    void SetEvents(const std::vector<EventData> &newEvents);
    [[nodiscard]] std::vector<EventData> GetEvents() const;

    void SetAngle(int newAngle);
    [[nodiscard]] int GetAngle() const;

    void SetSpeed(float newSpeed);
    [[nodiscard]] float GetSpeed() const;

    //Methods
    bool Move(float deltaTime, float dX, float dY, EntityCollisionComponent &collisionComponent, const Server* server);
    void MoveTo(float deltaTime, float targetX, float targetY, EntityCollisionComponent &collisionComponent, const Server* server);
    void Tick(const Server* server, IEntity &entity); //Process tasks when not already in progress else continue
    void AddEvent(const EventData &eventData);
    void RegisterScriptBinding(const std::string &scriptName, const ScriptData &scriptData);

    //Constructor
    explicit EntityLogicComponent(const Coordinates &coordinates);
};

class EntityHealthComponent {
    friend class EntityScripts;
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
    friend class EntityScripts;
    //Inventory data and methods would go here
    //To be implemented
};

class IEntity {
public:
    //Interface methods
    virtual void Tick() = 0;
    virtual RenderingContext GetRenderingContext() = 0;

    //Entity actions
    virtual void Move(float dX, float dY) = 0;

    //Setters
    virtual void SetCoordinates(const Coordinates &newCoordinates) = 0;
    //Sets entity angle in degrees
    virtual void SetAngle(int newAngle) = 0;
    virtual void SetSpeed(float newSpeed) = 0;
    virtual  void SetEntityCollision(bool disable) = 0;
    //Event
    virtual void AddEvent(const EventData &eventData) = 0;

    //Getters

    //Returns true entity coordinates (sprite center)
    [[nodiscard]] virtual Coordinates GetCoordinates() const = 0;
    //Returns entity collision status
    [[nodiscard]] virtual CollisionStatus GetCollisionStatus() const = 0;
    [[nodiscard]] virtual int GetAngle() const = 0;
    [[nodiscard]] virtual HitboxContext GetHitboxRenderingContext() const = 0;

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
