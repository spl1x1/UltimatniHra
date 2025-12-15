//
// Created by Lukáš Kaplánek on 25.10.2025.
//

/*
 *Vytvorit command dispatch thread pro entity, struktury atd.
 *access pres konzoli ingame
 */

#ifndef ENTITY_H
#define ENTITY_H
#include <memory>
#include <unordered_map>
#include <vector>
#include <SDL3/SDL_render.h>

#include "../Server/Server.h"
#include "../Sprites/Sprite.hpp"

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
    };
};

struct TaskData {
    enum class Type{
        MOVE_TO,
        GATHER_RESOURCE,
        BUILD_STRUCTURE,
        ATTACK,
        DIE
    };
    enum class Status {
        NEW,
        IN_PROGRESS,
        DONE,
        FAILED
    };

    Type type{};
    /*
    Parameters for the task
    */
    union {
        struct { float targetX{0}, targetY{0}; } moveTo;
        struct { int resourceId{0}, amount{0}; } gather;
        struct { int structureId{-1}; float x{0},y{0};} build;
    };
    //Task status
    Status status = Status::NEW;
};

class IEntity {
public:
    //Interface methods
    virtual ~IEntity() = 0;
    virtual void Tick(float deltaTime) = 0;
    virtual void Render(SDL_Renderer& windowRenderer, SDL_FRect& cameraRectangle, std::unordered_map<std::string, SDL_Texture*>& textures) = 0;
    virtual void Create() = 0;
    virtual void Load() = 0;

    //Entity actions
    virtual void Move(float dX, float dY) = 0;
    virtual void MoveTo(float targetX, float targetY) = 0;
    virtual void HandleAction(EventData data) = 0;

    //Setters
    virtual void SetCoordinates(const Coordinates &newCoordinates) = 0;
    //Sets entity angle in degrees
    virtual void SetAngle(float newAngle) = 0;
    virtual void SetSpeed(float newSpeed) = 0;
    //Sets current task and task data
    virtual void SetTask(TaskData data) = 0;
    virtual void RemoveTask(TaskData data) = 0;

    //Getters

    //Returns true entity coordinates (sprite center)
    [[nodiscard]] virtual Coordinates GetCoordinates() const = 0;
    //Returns entity collision status
    [[nodiscard]] virtual CollisionStatus HetCollisionStatus() const = 0;
    [[nodiscard]] virtual int GetAngle() const = 0;
    //Returns current task and task data
    [[nodiscard]] virtual TaskData GetTask() const = 0;

    //Disable copy constructor and allow move constructor
    IEntity(const IEntity&) =delete;
    IEntity(IEntity&&) = default;
};

class EntityRenderingComponent {
    std::unique_ptr<ISprite> sprite{nullptr};
    std::unique_ptr<SDL_FRect> Rect = std::make_unique<SDL_FRect>();

    public:
    void Render(const SDL_Renderer* renderer, const Coordinates &entityCoordinates, const SDL_FRect &cameraRectangle, std::unordered_map<std::string, SDL_Texture*> texturePool) const;
    void Tick(float deltaTime) const;

    //Setters
    void SetDirectionBaseOnAngle(int angle) const;

    //Constructor
    explicit EntityRenderingComponent(std::unique_ptr<ISprite> sprite): sprite(std::move(sprite)){}
    EntityRenderingComponent(const EntityRenderingComponent&) = delete;
    EntityRenderingComponent(EntityRenderingComponent&&) = default;
};

class EntityCollisionComponent {
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
    bool checkCollision(float newX, float newY, const std::shared_ptr<Server> &server);

    //Setters
    void SetHitbox(const HitboxData &hitbox);
    void disableCollision(bool Switch = true);

    //Getters
    [[nodiscard]] HitboxData* GetHitbox();
    [[nodiscard]] CollisionStatus GetCollisionStatus() const;

    //Constructor
    explicit EntityCollisionComponent(const HitboxData &hitbox): _hitbox(hitbox){}
};

class EntityMovementComponent {
    constexpr float threshold = 1.0f; //Threshold to consider reached target
    Coordinates coordinates{0.0f, 0.0f};
    std::vector<Coordinates> pathPoints{};
    std::vector<TaskData> tasks{};

    int angle{0};
    float speed{0};

    float currentDX{0};
    float currentDY{0};

    void SetAngleBasedOnMovement(float dX, float dY); //Sets angle based on movement direction
    void MakePath(float targetX, float targetY, const std::shared_ptr<Server> &server); //Generates pathPoints based on target
    void MoveTo(float targetX, float targetY); //Sets currentDX and currentDY based on target
    void PathMovement(); //Moves entity along pathPoints

public:

    //Setters and Getters
    void SetPathPoints(const std::vector<Coordinates> &newPathPoints);
    [[nodiscard]] std::vector<Coordinates> GetPathPoints() const;

    void SetCoordinates(const Coordinates &newCoordinates);
    [[nodiscard]] Coordinates GetCoordinates() const;

    void SetTasks(const std::vector<TaskData> &newTasks);
    [[nodiscard]] std::vector<TaskData> GetTasks() const;

    void SetAngle(int newAngle);
    [[nodiscard]] int GetAngle() const;

    //Methods
    static bool Move(float dX, float dY, EntityCollisionComponent &collisionComponent, Coordinates &entityCoordinates, const std::shared_ptr<Server> &server);
    void Tick(float deltaTime, const std::shared_ptr<Server> &server); //Process tasks when not already in progress else continue

    //Constructor
    explicit EntityMovementComponent(const Coordinates &coordinates): coordinates(coordinates){}
};

class EntityHealthComponent {
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
    //Inventory data and methods would go here
    //To be implemented
};

// -------------------------------------------------------


//Defines 4 hitbox corners, relative to sprite
struct Hitbox {
    Coordinates corners[4];
    bool disableCollision{false};
    bool colliding{false};
};

enum HitboxCorners{
    TOP_LEFT =0,
    TOP_RIGHT =1,
    BOTTOM_LEFT =2,
    BOTTOM_RIGHT =3
};


enum class EntityType{
    PLAYER,
    NPC,
    ANIMAL,
    MONSTER,
    TILE_ENTITY
};


class Entity {
    float offsetX= 0.0f;
    float offsetY= 0.0f;
    int angle = 0;
    std::shared_ptr<Server> server;

protected:
    void checkCollision(float newX, float newY);
    float speed = 0.0f;
    Hitbox hitbox {};

public:
    int id;
    virtual ~Entity() = default;
    std::unique_ptr<ISprite> sprite = nullptr;

    Coordinates coordinates;

    std::string name;
    EntityType type;

    float health;
    float maxHealth;


    virtual bool Move(float dX, float dY, float dt);

    //Setters
    void SetHitbox(const Hitbox &Hitbox){ this->hitbox = Hitbox;};
    void disableCollision(const bool Switch = true){hitbox.disableCollision = Switch;};
    void setSpeed(float newSpeed){ speed = newSpeed;};
    void setSpriteOffsetX(const float newOffsetX){ offsetX = newOffsetX;}
    void setSpriteOffsetY(const float newOffsetY){ offsetY = newOffsetY;}

    //Getters
    [[nodiscard]] int getAngle() const { return angle;}
    [[nodiscard]] float GetSpeed() const { return speed;}
    [[nodiscard]] Hitbox* GetHitbox() { return &hitbox;}
    [[nodiscard]] bool collisionDisabled() const {return hitbox.disableCollision;}
    [[nodiscard]] bool isColliding() const {return hitbox.colliding;}
    //Returns sprite center
    [[nodiscard]] Coordinates getTrueCoordinates() const { return Coordinates{coordinates.x + offsetX, coordinates.y + offsetY};}

    // Base Entity class, all entities inherit from this
    Entity(int id, float maxHealth, Coordinates coordinates, EntityType type, const std::shared_ptr<Server> &server, float speed=0.0f, std::unique_ptr<ISprite> sprite = nullptr);
};


#endif //ENTITY_H
