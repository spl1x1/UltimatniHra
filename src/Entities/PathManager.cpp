//
// Created by USER on 27.12.2025.
//
#include "../../include/Entities/PathManager.h"

#include <algorithm>
#include <cmath>
#include <queue>
#include <ranges>

#include "../../include/Entities/Entity.h"
#include "../../include/Application/MACROS.h"

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

std::unordered_map<int, PathManager> PathManager::activePathFinds{};

std::vector<Coordinates> PathManager::MakePath(const float targetX, const float targetY, IEntity &entity) {
    const auto logicComponent{entity.GetLogicComponent()};

    std::vector<Coordinates> pathPoints;
    const auto coordinates {logicComponent->GetCoordinates()};

    const auto startX{static_cast<int>(std::floor(coordinates.x / 32.0f))};
    const auto startY{static_cast<int>(std::floor(coordinates.y / 32.0f))};
    const auto endX{static_cast<int>(std::floor (targetX / 32.0f))};
    const auto endY{static_cast<int>(std::floor (targetY / 32.0f))};

    if (endX < 0 || endY < 0 || endX >= MAPSIZE || endY >= MAPSIZE) return {};
    if (EntityCollisionComponent::CheckCollisionAtTile(endX, endY, entity)) return {};

    auto heuristic = [](const int x1, const int y1, const int x2, const int y2) -> float {
        const auto dx{std::abs(x1 - x2)};
        const auto dy {std::abs(y1 - y2)};
        return static_cast<float>(std::max(dx, dy)) + (1.414f - 1.0f) * static_cast<float>(std::min(dx, dy));
    };

    std::priority_queue<PathNode, std::vector<PathNode>, std::greater<>> pq;
    std::unordered_map<GridPoint, bool, GridPointHash> visited;
    std::unordered_map<GridPoint, GridPoint, GridPointHash> parent;
    std::unordered_map<GridPoint, float, GridPointHash> gCost;

    GridPoint start{startX, startY};
    GridPoint end{endX, endY};

    const auto startHeuristic = heuristic(startX, startY, endX, endY);
    pq.push({startX, startY, startHeuristic});
    gCost[start] = 0.0f;

    auto pathFound{false};

    while (!pq.empty()) {
        PathNode current{pq.top()};
        pq.pop();

        GridPoint currentPoint{current.x, current.y};

        if (visited[currentPoint]) continue;
        visited[currentPoint] = true;

        if (current.x == endX && current.y == endY) {
            pathFound = true;
            break;
        }

        for (int i = 0; i < 8; i++) {
            constexpr float moveCost[] = {1.0f, 1.0f, 1.0f, 1.0f, 1.414f, 1.414f, 1.414f, 1.414f};
            constexpr int dy[]{0, 0, -1, 1, -1, 1, -1, 1};
            constexpr int dx[]{-1, 1, 0, 0, -1, -1, 1, 1};
            const auto nx{current.x + dx[i]};
            const auto ny{current.y + dy[i]};
            GridPoint neighbour{nx, ny};

            if (nx < 0 || ny < 0 || nx >= MAPSIZE || ny >= MAPSIZE) continue;
            if (EntityCollisionComponent::CheckCollisionAtTile(nx, ny, entity)) continue;
            if (visited[neighbour]) continue;

            // Kontrola diagonálního pohybu
            if (i >= 4) {
                const auto adjX1{current.x + dx[i]};
                const auto adjY1{current.y};
                const auto adjX2{current.x};
                const auto adjY2{current.y + dy[i]};

                if (EntityCollisionComponent::CheckCollisionAtTile(adjX1, adjY1, entity) ||
                    EntityCollisionComponent::CheckCollisionAtTile(adjX2, adjY2, entity)) {
                    continue;
                }
            }

            if (const auto newGCost{gCost[currentPoint] + moveCost[i]}; !gCost.contains(neighbour) || newGCost < gCost[neighbour]) {
                gCost[neighbour] = newGCost;
                parent[neighbour] = currentPoint;
                const auto fCost = newGCost + heuristic(nx, ny, endX, endY);
                pq.push({nx, ny, fCost});
            }
        }
    }

    if (pathFound) {
        std::vector<GridPoint> fullPath;
        GridPoint current{end};

        while (current != start) {
            fullPath.push_back(current);
            current = parent[current];
        }
        fullPath.push_back(start);

        std::ranges::reverse(fullPath);

        if (fullPath.size() > 1) {
            pathPoints.push_back({
                static_cast<float>(fullPath.at(0).x * 32 + 16),
                static_cast<float>(fullPath.at(0).y * 32 + 16)
            });

            for (size_t i{1}; i < fullPath.size() - 1; i++) {
                const auto prevDx{fullPath[i].x - fullPath[i-1].x};
                const auto prevDy{fullPath[i].y - fullPath[i-1].y};
                const auto nextDx{fullPath[i+1].x - fullPath[i].x};
                const auto nextDy{fullPath[i+1].y - fullPath[i].y};

                if (prevDx != nextDx || prevDy != nextDy) {
                    pathPoints.push_back({
                        static_cast<float>(fullPath[i].x * 32 + 16),
                        static_cast<float>(fullPath[i].y * 32 + 16)
                    });
                }
            }

            pathPoints.push_back({
                static_cast<float>(fullPath.back().x * 32 + 16),
                static_cast<float>(fullPath.back().y * 32 + 16)
            });
        }
    }

    return pathPoints;
}


PathManager::PathManager(IEntity &entity, Coordinates targetCoordinates) : entity(entity) {

    const auto entityCoordinates{entity.GetLogicComponent()->GetCoordinates()};
    while (EntityCollisionComponent::CheckCollisionAt(targetCoordinates.x, targetCoordinates.y, entity)) {
        if (entityCoordinates.x == targetCoordinates.x && entityCoordinates.y == targetCoordinates.y) {
            break;
        }

        if (targetCoordinates.x > entityCoordinates.x) {
            targetCoordinates.x -= 1.0f;
        } else if (targetCoordinates.x < entityCoordinates.x) {
            targetCoordinates.x += 1.0f;
        }

        if (targetCoordinates.y > entityCoordinates.y) {
            targetCoordinates.y -= 1.0f;
        } else if (targetCoordinates.y < entityCoordinates.y) {
            targetCoordinates.y += 1.0f;
        }
    };

    pathPoints = MakePath(targetCoordinates.x, targetCoordinates.y, entity);
}

void PathManager::Tick() {
    std::vector<int> completedEntries;
    completedEntries.reserve(activePathFinds.size());

    for (auto &manager: activePathFinds | std::views::values) {
        if (manager.pathPoints.empty()) {
            manager.completed = true;
        }
        if (manager.IsCompleted()) {
            completedEntries.emplace_back(manager.entity.GetId());
            continue;
        }
        if (manager.IsPaused()) continue;

        if (manager.entity.GetCoordinates() - manager.pathPoints.front() < EntityLogicComponent::threshold) {
            manager.pathPoints.erase(manager.pathPoints.begin());
        }

        if (manager.pathPoints.empty()) {
            manager.completed = true;
            completedEntries.emplace_back(manager.entity.GetId());
            continue;
        }

        if (manager.currentPoint == manager.pathPoints.front()) continue;

        manager.currentPoint = manager.pathPoints.front();
        manager.entity.GetLogicComponent()->AddEvent(Event_MoveTo::Create(manager.pathPoints.front()));
    }

    for (const auto& entityId : completedEntries) {
        activePathFinds.erase(entityId);
    }
}


void PathManager::StartPathfinding(const Coordinates targetCoordinates, IEntity &entity) {
    if (IsPathfindingActive(entity)) return;
    activePathFinds.emplace(entity.GetId(), PathManager(entity, targetCoordinates));
}

void PathManager::PausePathfinding(const IEntity &entity) {
    if (!IsPathfindingActive(entity)) return;
    activePathFinds.at(entity.GetId()).paused = true;
}

void PathManager::ResumePathfinding(const IEntity &entity) {
    if (!IsPathfindingActive(entity)) return;
    activePathFinds.at(entity.GetId()).paused = false;
}

void PathManager::StopPathfinding(const IEntity &entity) {
    if (!IsPathfindingActive(entity)) return;
    activePathFinds.erase(entity.GetId());
}

bool PathManager::IsPathfindingActive(const IEntity &entity) {
    return activePathFinds.contains(entity.GetId());
}

bool PathManager::IsPaused() const {
    return paused;
}

bool PathManager::IsCompleted() const{
    return completed;
}
