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
#include <queue>
#include <unordered_map>
#include <vector>
#include <SDL3/SDL_render.h>

#include "../Server/Server.h"
#include "../Sprites/Sprite.hpp"

class IEntity;

enum class Event{
    // data = float dX, float dY
    MOVE,
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
    HEAL
};

struct EventData {
    Event type{};
    //Delta time for task processing
    float dt{};
    /*
     Parameters for the event
     */
    union {
        struct { float dX{0}, dY{0}; } move;
        struct { int resourceId{0}, amount{0}; } gather;
        struct { int structureId{-1}; float x{0},y{0};} build;
        struct { int attackType{0}; } attack;
        struct { float amount{0}; } healthChange;
    } data;
};

struct TaskData {
    enum class Status {
        PENDING,
        IN_PROGRESS,
        DONE,
        FAILED
    };

    std::string taskName{};
    /*
    Parameters for the task
    */
    union {
        struct { float targetX{0}, targetY{0};} moveTo;
        struct { int resourceId{0}, amount{0}; } gather;
        struct { int structureId{-1}; float x{0},y{0};} build;
    };
    std::vector<Coordinates> pathPoints{};
    //Task status
    Status status = Status::PENDING;
};

class EntityRenderingComponent {
    friend class EntityScripts;
    std::unique_ptr<ISprite> _sprite{nullptr};
    std::unique_ptr<SDL_FRect> _rect;

public:
    void Render(const SDL_Renderer* renderer, const Coordinates &entityCoordinates, const SDL_FRect &cameraRectangle, std::unordered_map<std::string, SDL_Texture*> texturePool) const;
    void Tick(float deltaTime) const;

    //Setters
    void SetDirectionBaseOnAngle(int angle) const;
    void SetAnimation(AnimationType animation) const;
    void SetSprite(std::unique_ptr<ISprite> sprite);

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
        bool disableCollision{false};
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

    //Getters
    [[nodiscard]] HitboxData* GetHitbox();
    [[nodiscard]] CollisionStatus GetCollisionStatus() const;

    //Constructor
    explicit EntityCollisionComponent(const HitboxData &hitbox): _hitbox(hitbox){}

};

class EntityLogicComponent {
    friend class EntityScripts;
public:

    struct ScriptData {
        std::string scriptName{};
        std::function<void(IEntity*, TaskData*)> function{};
    };

private:
    static constexpr float threshold = 1.0f; //Threshold to consider reached target
    Coordinates _coordinates{0.0f, 0.0f};
    std::vector<TaskData> _tasks{};
    std::vector<EventData> _events{};

    std::unordered_map<std::string,ScriptData> _scriptBindings; //Scripts bound to entity

    int _angle{0};
    float _speed{0};

    void HandleEvent(const Server* server, EntityCollisionComponent &collisionComponent);
    void HandleTask(const Server* server, EntityCollisionComponent &collisionComponent, IEntity* entity);
    void ProcessNewTask(const Server* server, EntityCollisionComponent &collisionComponent, IEntity* entity);
    void SetAngleBasedOnMovement(float dX, float dY); //Sets angle based on movement direction

public:

    //Script binding methods
    void BindScript(const std::string &scriptName, const ScriptData &scriptData);
    void UnbindScript(const std::string &scriptName);
    ScriptData GetBoundScript(const std::string &scriptName) const;

    void SetCoordinates(const Coordinates &newCoordinates);
    [[nodiscard]] Coordinates GetCoordinates() const;

    void SetTasks(const std::vector<TaskData> &newTasks);
    void SetTask(const TaskData &newTask);
    [[nodiscard]] std::vector<TaskData> GetTasks() const;

    void SetEvents(const std::vector<EventData> &newEvents);
    [[nodiscard]] std::vector<EventData> GetEvents() const;

    void SetAngle(int newAngle);
    [[nodiscard]] int GetAngle() const;

    void SetSpeed(float newSpeed);
    [[nodiscard]] float GetSpeed() const;

    //Methods
    bool Move(float deltaTime, float dX, float dY, EntityCollisionComponent &collisionComponent, const Server* server);
    void Tick(float deltaTime, const Server* server, EntityCollisionComponent &collisionComponent, IEntity *entity); //Process tasks when not already in progress else continue
    void AddEvent(const EventData &eventData);
    void RegisterScriptBinding(const std::string &scriptName, const ScriptData &scriptData);

    //Constructor
    explicit EntityLogicComponent(const Coordinates &coordinates);
};

class EntityHealthComponent {
    friend class EntityScripts;
    float health{0.0f};
    float maxHealth{0.0f};
public:
    //Methods
    void TakeDamage(float damage);
    void Heal(float amount);

    //Setters
    void SetHealth(float newHealth);
    void SetMaxHealth(float newMaxHealth);

    //Getters
    [[nodiscard]] float GetHealth() const;
    [[nodiscard]] float GetMaxHealth() const;
    [[nodiscard]] bool isDead() const;

    //Constructor
    EntityHealthComponent(const float health, const float maxHealth): health(health), maxHealth(maxHealth){}
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
    virtual void Render(SDL_Renderer& windowRenderer, SDL_FRect& cameraRectangle, std::unordered_map<std::string, SDL_Texture*>& textures) = 0;

    //Entity actions
    virtual void Move(float dX, float dY) = 0;
    virtual void HandleTask(TaskData data) = 0;

    //Setters
    virtual void SetCoordinates(const Coordinates &newCoordinates) = 0;
    //Sets entity angle in degrees
    virtual void SetAngle(int newAngle) = 0;
    virtual void SetSpeed(float newSpeed) = 0;
    //Sets current task and task data
    virtual void SetTask(int index) = 0;
    virtual void RemoveTask(int index) = 0;
    virtual  void SetEntityCollision(bool disable) = 0;
    //Event
    virtual void AddEvent(const EventData &eventData) = 0;

    //Getters

    //Returns true entity coordinates (sprite center)
    [[nodiscard]] virtual Coordinates GetCoordinates() const = 0;
    //Returns entity collision status
    [[nodiscard]] virtual CollisionStatus GetCollisionStatus() const = 0;
    [[nodiscard]] virtual int GetAngle() const = 0;
    //Returns current task and task data

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
