//
// Created by USER on 27.12.2025.
//

#ifndef ENTITYSCRIPTS_H
#define ENTITYSCRIPTS_H

struct EventData;
class IEntity;

class EntityScripts {
    //Sub functions for scripts --- MoveToScript
    static void MakePath(float targetX, float targetY,IEntity &entity, EventData &eventData);
    static bool MoveTo(IEntity &entity, const EventData &eventData);

public:
    static void MoveToScript(IEntity &entity, EventData &eventData){};

};

#endif //ENTITYSCRIPTS_H
