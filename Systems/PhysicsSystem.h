#ifndef PHYSICSSYSTEM_H
#define PHYSICSSYSTEM_H

#include "ECS.h"
#include "Components/PhysicsComponent.h"

class PhysicsSystem : public System<PhysicsSystem>
{
public:
    PhysicsSystem();

    void update(float dt);
    void handleEvent(BaseEvent* event);

private:
    EntitySet *m_entities;
    std::vector<PhysicsComponent> *m_components;
};

#endif // PHYSICSSYSTEM_H
