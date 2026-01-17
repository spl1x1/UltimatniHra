//
// Created by USER on 27.12.2025.
//

#ifndef ENTITYSCRIPTS_H
#define ENTITYSCRIPTS_H
#include <set>

#include "Entity.h"
#include "../Application/dataStructures.h"

class IEntity;

class PathManager {
    //Static methods and members
    static std::vector<Coordinates> MakePath(float targetX, float targetY,IEntity &entity);
    static std::unordered_map<int, PathManager> activePathFinds;

    //Instance members and methods
    IEntity& entity;
    std::vector<Coordinates> pathPoints;
    Coordinates currentPoint;

    bool paused{false};
    bool completed{false};

    [[nodiscard]] bool IsPaused() const;
    [[nodiscard]] bool IsCompleted() const;

    PathManager(IEntity& entity, Coordinates targetCoordinates);

public:
    static void Tick();
    static void StartPathfinding(Coordinates targetCoordinates, IEntity &entity);
    static void PausePathfinding(const IEntity &entity);
    static void ResumePathfinding(const IEntity &entity);
    static void StopPathfinding(const IEntity &entity);

    static bool IsPathfindingActive(const IEntity &entity);

};

#endif //ENTITYSCRIPTS_H
