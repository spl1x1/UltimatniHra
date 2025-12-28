//
// Created by USER on 27.12.2025.
//

#ifndef ENTITYSCRIPTS_H
#define ENTITYSCRIPTS_H

struct TaskData;
class IEntity;

class EntityScripts {
    //Sub functions for scripts --- MoveToScript
    static void MakePath(float targetX, float targetY,IEntity* entity, TaskData* taskData);
    static bool MoveTo(IEntity* entity, const TaskData *taskData);

public:
    static void MoveToScript(IEntity* entity, TaskData *taskData);

};

#endif //ENTITYSCRIPTS_H
