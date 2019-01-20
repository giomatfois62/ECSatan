#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H

#include "ECS.h"
#include "Components/PhysicsComponent.h"

class CollisionSystem : public System<CollisionSystem>
{
public:
    CollisionSystem();

    void handleEvent(BaseEvent* event);
};

#endif // COLLISIONSYSTEM_H
