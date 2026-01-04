//
// Created by USER on 03.01.2026.
//

#ifndef ENTITYEVENT_H
#define ENTITYEVENT_H
#include <memory>

//Abstract event class for entity events
class EntityEvent {
public:
    enum class Type{
        NONE,
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
        CLICK_MOVE,
        //interupts type of event sent right after
        INTERRUPT_SPECIFIC
    };
protected:
    Type type{Type::NONE};
    float deltaTime{0};
    [[nodiscard]] bool validateDeltaTime() const; // Validate that delta not 0 or negative for events that require it
public:
    [[nodiscard]] Type GetType() const; // Get event type
    void SetDeltaTime(float dt); // Delta time is set by the server when processing events
    [[nodiscard]] float GetDeltaTime() const; // Get delta time
    [[nodiscard]] virtual bool validate() const; // Validate event data, to be overridden by derived classes, default implementation returns true


    virtual ~EntityEvent() = default;
};

class Event_Move final : public EntityEvent {
public:
    float dX{0};
    float dY{0};

    explicit Event_Move(float dX, float dY);
    [[nodiscard]] bool validate() const override;
    [[nodiscard]]  static std::unique_ptr<EntityEvent> Create(float dX, float dY);
};

class Event_MoveTo final : public EntityEvent {
public:
    float targetX{-1};
    float targetY{-1};

    explicit Event_MoveTo(float targetX, float targetY);
    [[nodiscard]] bool validate() const override;
    [[nodiscard]]  static std::unique_ptr<EntityEvent> Create(float targetX, float targetY);
};

class Event_ChangeCollision final : public EntityEvent {
public:
    explicit Event_ChangeCollision();
    [[nodiscard]]  static std::unique_ptr<EntityEvent> Create();
};

class Event_Interrupt final : public EntityEvent {
public:
    explicit Event_Interrupt();
    [[nodiscard]]  static std::unique_ptr<EntityEvent> Create();
};

class Event_Damage final : public EntityEvent {
public:
    int amount{-1};
    explicit Event_Damage(int amount);
    [[nodiscard]] bool validate() const override;
    [[nodiscard]]  static std::unique_ptr<EntityEvent> Create(int amount);
};

class Event_Heal final : public EntityEvent {
public:
    int amount{-1};
    explicit Event_Heal(int amount);
    [[nodiscard]] bool validate() const override;
    [[nodiscard]]  static std::unique_ptr<EntityEvent> Create(int amount);
};

class Event_ClickMove final : public EntityEvent {
public:
    float targetX{-1};
    float targetY{-1};
    explicit Event_ClickMove(float targetX, float targetY);
    [[nodiscard]] bool validate() const override;
    [[nodiscard]]  static std::unique_ptr<EntityEvent> Create(float targetX, float targetY);
};

class Event_InterruptSpecific final : public EntityEvent {
public:
    Type eventToInterrupt{Type::NONE};
    explicit Event_InterruptSpecific(Type eventType);

    [[nodiscard]] bool validate() const override;
    [[nodiscard]] static std::unique_ptr<EntityEvent> Create(Type eventType);
};


#endif //ENTITYEVENT_H
