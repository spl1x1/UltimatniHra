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
#include <queue>
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
        struct { float targetX{0}, targetY{0}; } moveTo;
        struct { int resourceId{0}, amount{0}; } gather;
        struct { int structureId{-1}; float x{0},y{0};} build;
    };
    int Iterations{0};
    //Task status
    Status status = Status::PENDING;
};

class IEntity {
public:
    //Interface methods
    virtual void Tick() = 0;
    virtual void Render(SDL_Renderer& windowRenderer, SDL_FRect& cameraRectangle, std::unordered_map<std::string, SDL_Texture*>& textures) = 0;
    virtual void Create() = 0;
    virtual void Load() = 0;

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

    //Getters

    //Returns true entity coordinates (sprite center)
    [[nodiscard]] virtual Coordinates GetCoordinates() const = 0;
    //Returns entity collision status
    [[nodiscard]] virtual CollisionStatus GetCollisionStatus() const = 0;
    [[nodiscard]] virtual int GetAngle() const = 0;
    //Returns current task and task data
    [[nodiscard]] virtual TaskData GetTask() const = 0;
    //Returns task queue
    [[nodiscard]] virtual std::vector<TaskData> GetTasks() const = 0;
    //Returns event queue
    [[nodiscard]] virtual std::vector<EventData> GetEvents() const = 0;

    //Disable copy constructor and allow move constructor
    IEntity(const IEntity&) =delete;
    virtual ~IEntity() = default;
    IEntity() = default;
};

class EntityRenderingComponent {
    std::unique_ptr<ISprite> _sprite{nullptr};
    std::unique_ptr<SDL_FRect> _rect = std::make_unique<SDL_FRect>();

public:
    void Render(const SDL_Renderer* renderer, const Coordinates &entityCoordinates, const SDL_FRect &cameraRectangle, std::unordered_map<std::string, SDL_Texture*> texturePool) const;
    void Tick(float deltaTime) const;

    //Setters
    void SetDirectionBaseOnAngle(int angle) const;
    void SetAnimation(AnimationType animation) const;
    void SetSprite(std::unique_ptr<ISprite> sprite);

    //Constructor
    explicit EntityRenderingComponent(std::unique_ptr<ISprite> sprite): _sprite(std::move(sprite)){}
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
    bool CheckCollision(float newX, float newY, const std::shared_ptr<Server> &server);
    [[nodiscard]] bool CheckCollisionAt(float newX, float newY, const std::shared_ptr<Server> &server) const;

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
public:
    enum class Script {
        NOT_IMPLEMENTED,
        LUA,
        CPP
    };

    struct ScriptData {
        Script script{Script::NOT_IMPLEMENTED};
        std::vector<std::function<void(EntityLogicComponent&)>> functionsCPP{};
        std::vector<std::string> scriptFilesLua{};
    struct {
        int maxIterations{0};
        bool isIterative{false};
    } iterationData;
    };

private:
    static constexpr float threshold = 1.0f; //Threshold to consider reached target
    Coordinates _coordinates{0.0f, 0.0f};
    std::vector<Coordinates> _pathPoints{};
    std::vector<TaskData> _tasks{};
    std::vector<EventData> _events{};

    std::unordered_map<std::string,ScriptData> _scriptBindings; //Scripts bound to entity

    int _angle{0};
    float _speed{0};

    struct PathNode {
        int x, y;
        float cost;
        bool operator>(const PathNode& other) const {
            return cost > other.cost;
        }
    };

    struct GridPoint {
        int x, y;
        bool operator==(const GridPoint& other) const {
            return x == other.x && y == other.y;
        }
    };

    struct GridPointHash {
        size_t operator()(const GridPoint& p) const {
            return std::hash<int>()(p.x) ^ (std::hash<int>()(p.y) << 1);
        }
    };

    float oldDX{0};
    float oldDY{0};

    void HandleEvent(const EventData &data, const std::shared_ptr<Server> &server, EntityCollisionComponent &collisionComponent);
    void HandleTask(const TaskData &data, const std::shared_ptr<Server> &server, EntityCollisionComponent &collisionComponent);
    void ProcessNewTask(const std::shared_ptr<Server> &server, EntityCollisionComponent &collisionComponent);
    void SetAngleBasedOnMovement(float dX, float dY); //Sets angle based on movement direction
    void MakePath(float targetX, float targetY, const std::shared_ptr<Server> &server, const EntityCollisionComponent &collisionComponent); //Generates pathPoints based on target
    void MoveTo(float targetX, float targetY); //Sets currentDX and currentDY based on target
    void PathMovement(EntityCollisionComponent &collisionComponent, const std::shared_ptr<Server> &server); //Moves entity along pathPoints

public:

    //Script binding methods
    void BindScript(const std::string &scriptName, const ScriptData &scriptData);
    void UnbindScript(const std::string &scriptName);
    ScriptData GetBoundScript(const std::string &scriptName) const;

    //Setters and Getters
    void SetPathPoints(const std::vector<Coordinates> &newPathPoints);
    [[nodiscard]] std::vector<Coordinates> GetPathPoints() const;

    void SetCoordinates(const Coordinates &newCoordinates);
    [[nodiscard]] Coordinates GetCoordinates() const;

    void SetTasks(const std::vector<TaskData> &newTasks);
    void SetTask(const TaskData &newTask);
    [[nodiscard]] std::vector<TaskData> GetTasks() const;
    [[nodiscard]] TaskData GetTask(int index) const;

    void SetEvents(const std::vector<EventData> &newEvents);
    [[nodiscard]] std::vector<EventData> GetEvents() const;

    void SetAngle(int newAngle);
    [[nodiscard]] int GetAngle() const;

    void SetSpeed(float newSpeed);
    [[nodiscard]] float GetSpeed() const;

    //Methods
    bool Move(float dX, float dY, EntityCollisionComponent &collisionComponent, const std::shared_ptr<Server> &server);
    void Tick(float deltaTime, const std::shared_ptr<Server> &server, EntityCollisionComponent &collisionComponent); //Process tasks when not already in progress else continue
    void SwitchTask(int index);
    void RemoveTask(int index);
    void AddEvent(const EventData &eventData);

    //Constructor
    explicit EntityLogicComponent(const Coordinates &coordinates): _coordinates(coordinates){}
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
