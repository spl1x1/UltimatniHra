// include/Server/AiManager.h
#ifndef ULTIMATNIHRA_AIMANAGER_H
#define ULTIMATNIHRA_AIMANAGER_H

#include <unordered_map>
#include <functional>
#include <memory>
#include <queue>

class Server;
class IEntity;

enum class AiState {
    Idle,
    Patrol,
    Chase,
    GetUnstuck, // Obecny stav pro reseni zaseknuti
    Attack,
    Flee,
    Dead
};

enum class AiEvent {
    PlayerSpotted,
    PlayerLost,
    TookDamage,
    LowHealth,
    TargetInRange,
    TargetOutOfRange,
    ReachedDestination,
    MovementStuck
};

class AiStateMachine {
public:
    using StateHandler = std::function<void(IEntity*, float deltaTime)>;

    void setState(AiState state);
    AiState getState() const;

    void registerState(AiState state, const StateHandler& onEnter, const StateHandler& onUpdate);

    void registerTransition(AiState from, AiEvent event, AiState to);

    void handleEvent(AiEvent event);
    void update(IEntity *entity, float deltaTime);

private:
    bool stateChanged = false;
    AiState currentState = AiState::Idle;
    AiState previousState = AiState::Idle; // Pro detekci změny stavu
    std::unordered_map<AiState, StateHandler> stateEnterHandlers; // onEnter
    std::unordered_map<AiState, StateHandler> stateHandlers;      // onUpdate
    std::unordered_map<AiState, std::unordered_map<AiEvent, AiState>> transitions;
};


class AiManager {
public:
    void registerEntity(IEntity* entity);
    void unregisterEntity(IEntity* entity);

    void sendEvent(IEntity* entity, AiEvent event);
    void update(float deltaTime);

    AiStateMachine* getStateMachine(IEntity *entity) const;

private:
    std::unordered_map<IEntity*, std::unique_ptr<AiStateMachine>> entityStates;
    std::queue<std::pair<IEntity*, AiEvent>> eventQueue;
};

#endif //ULTIMATNIHRA_AIMANAGER_H
