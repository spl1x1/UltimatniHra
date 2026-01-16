//
// Created by Lukáš Kaplánek on 16.01.2026.
//
// src/Server/AiManager.cpp
#include "../../include/Server/AiManager.h"
#include "../../include/Entities/Entity.h"

// AiStateMachine
void AiStateMachine::setState(const AiState state) {
    currentState = state;
}

AiState AiStateMachine::getState() const { return currentState; }

void AiStateMachine::registerState(const AiState state, const StateHandler &onUpdate) {
    stateHandlers.at(state) = onUpdate;
}

void AiStateMachine::registerTransition(const AiState from, const AiEvent event, const AiState to) {
    transitions.at(from).at(event) = to;
}

void AiStateMachine::handleEvent(const AiEvent event) {
    if (const auto& stateTransitions{transitions[currentState]}; stateTransitions.contains(event)) {
        currentState = stateTransitions.at(event);
    }
}

void AiStateMachine::update(IEntity* entity, const float deltaTime) const {
    if (stateHandlers.contains(currentState)) {
        stateHandlers.at(currentState)(entity, deltaTime);
    }
}

// AiManager
void AiManager::registerEntity(IEntity* entity) {
    entityStates.at(entity) = std::make_unique<AiStateMachine>();
}

void AiManager::unregisterEntity(IEntity* entity) {
    entityStates.erase(entity);
}

void AiManager::sendEvent(IEntity* entity, AiEvent event) {
    eventQueue.push({entity, event});
}

AiStateMachine* AiManager::getStateMachine(IEntity* entity) const {
    if (entityStates.contains(entity)) {
        return entityStates.at(entity).get();
    }
    return nullptr;
}

void AiManager::update(const float deltaTime) {
    // Handle queued events
    while (!eventQueue.empty()) {
        auto [entity, event] = eventQueue.front();
        eventQueue.pop();
        if (auto* sm = getStateMachine(entity)) {
            sm->handleEvent(event);
        }
    }

    // Aktualizace vsech automatu
    for (auto& [entity, stateMachine] : entityStates) {
        stateMachine->update(entity, deltaTime);
    }
}
