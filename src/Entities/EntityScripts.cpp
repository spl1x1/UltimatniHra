//
// Created by USER on 27.12.2025.
//
#include "../../include/Entities/EntityScripts.h"

#include <algorithm>
#include <cmath>
#include <queue>

#include "../../include/Entities/Entity.h"
#include "../../include/Application/MACROS.h"

void EntityScripts::MoveToScript(IEntity &entity, TaskData &taskData) {
    if (taskData.status  == TaskData::Status::PENDING) {
        MakePath(taskData.moveTo.targetX, taskData.moveTo.targetY, entity, taskData);
    }
    if (MoveTo(entity, taskData)) {
        taskData.pathPoints.erase(taskData.pathPoints.begin());
    }
    if (taskData.pathPoints.empty()) {
        taskData.status = TaskData::Status::DONE;
    }
}

bool EntityScripts::MoveTo(IEntity &entity, const TaskData &taskData) {
    const auto logicComponent = entity.GetLogicComponent();
    const Coordinates targetPoint = taskData.pathPoints.front();
    const Coordinates currentPoint = logicComponent->GetCoordinates();

    const float deltaX{targetPoint.x - currentPoint.x};
    const float deltaY{targetPoint.y - currentPoint.y};
    const float distance = std::sqrt(deltaX * deltaX + deltaY * deltaY);

    if (!(distance > EntityLogicComponent::threshold)) {
        return true;
    }

    logicComponent->AddEvent(EventData{
            .type = Event::MOVE,
            .data = {deltaX / distance, deltaY / distance}
    });
    return false;
}


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

void EntityScripts::MakePath(const float targetX, const float targetY, IEntity &entity, TaskData &taskData) {
    const auto logicComponent = entity.GetLogicComponent();
    const auto collisionComponent = *entity.GetCollisionComponent();
    const auto server = entity.GetServer();

    taskData.pathPoints.clear();

    const int startX = static_cast<int>(std::floor(logicComponent->_coordinates.x / 32.0f));
    const int startY = static_cast<int>(std::floor(logicComponent->_coordinates.y / 32.0f));
    const int endX = static_cast<int>(std::floor (targetX / 32.0f));
    const int endY = static_cast<int>(std::floor (targetY / 32.0f));

    if (endX <0 || endY <0 || endX >= MAPSIZE || endY >= MAPSIZE) return;
    if (collisionComponent.CheckCollisionAt(static_cast<float>(endX), static_cast<float>(endY), entity.GetServer())) return;

    std::priority_queue<PathNode, std::vector<PathNode>, std::greater<>> pq;
    std::unordered_map<GridPoint, bool, GridPointHash> visited;
    std::unordered_map<GridPoint, GridPoint, GridPointHash> parent;
    std::unordered_map<GridPoint, float, GridPointHash> cost;

    GridPoint start = {startX, startY};
    GridPoint end = {endX, endY};

    pq.push({startX, startY, 0.0f});
    cost[start] = 0.0f;

    bool pathFound = false;

    while (!pq.empty()) {
        PathNode current = pq.top();
        pq.pop();

        GridPoint currentPoint = {current.x, current.y};

        if (visited[currentPoint]) continue;
        visited[currentPoint] =true;

        if (current.x == endX && current.y == endY) {
            pathFound = true;
            break;
        }
        for (int i = 0; i < 8;i++) {
            constexpr int dx[] = {-1, 1, 0, 0, -1, -1, 1, 1};
            constexpr int dy[] = {0, 0, -1, 1, -1, 1, -1, 1};
            constexpr float moveCost[] = {1.0f, 1.0f, 1.0f, 1.0f, 1.414f, 1.414f, 1.414f, 1.414f};
            const int nx = current.x + dx[i];
            const int ny = current.y + dy[i];
            GridPoint neighbour = {nx, ny};

            if (nx < 0 || ny < 0 || nx >= MAPSIZE || ny >= MAPSIZE) continue;

            if (collisionComponent.CheckCollisionAt(static_cast<float>(nx),static_cast<float>(ny),server)) continue;
            if (visited[neighbour]) continue;

            if (i >= 4) {
                const int adjX1 = current.x + dx[i];
                const int adjY1 = current.y;
                const int adjX2 = current.x;
                const int adjY2 = current.y + dy[i];

                if (collisionComponent.CheckCollisionAt(static_cast<float>(adjX1),static_cast<float>(adjY1),server) ||
                    collisionComponent.CheckCollisionAt(static_cast<float>(adjX2),static_cast<float>(adjY2),server)) {
                    continue;
                    }
            }
            const float newCost = current.cost + moveCost[i];

            if (!cost.contains(neighbour) || newCost < cost[neighbour]) {
                cost[neighbour] = newCost;
                parent[neighbour] = currentPoint;
                pq.push({nx, ny, newCost});
            }
        }
    }
    if (pathFound) {
        std::vector<GridPoint> fullPath;
        GridPoint current = end;

        while (current != start) {
            fullPath.push_back(current);
            current = parent[current];
        }
        fullPath.push_back(start);

        std::ranges::reverse(fullPath);

        if (fullPath.size() > 1) {
                taskData.pathPoints.push_back({
                static_cast<float>(fullPath[0].x * 32 + 16),
                static_cast<float>(fullPath[0].y * 32 + 16)
            });

            for (size_t i = 1; i < fullPath.size() - 1; i++) {
                const int prevDx = fullPath[i].x - fullPath[i-1].x;
                const int prevDy = fullPath[i].y - fullPath[i-1].y;
                const int nextDx = fullPath[i+1].x - fullPath[i].x;
                const int nextDy = fullPath[i+1].y - fullPath[i].y;

                if (prevDx != nextDx || prevDy != nextDy) {
                    taskData.pathPoints.push_back({
                        static_cast<float>(fullPath[i].x * 32 + 16),
                        static_cast<float>(fullPath[i].y * 32 + 16)
                    });
                }
            }

            taskData.pathPoints.push_back({
                static_cast<float>(fullPath.back().x * 32 + 16),
                static_cast<float>(fullPath.back().y * 32 + 16)
            });
        }
    }


}
