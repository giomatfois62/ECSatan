#ifndef PHYSICSCOMPONENT_H
#define PHYSICSCOMPONENT_H

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include "ECS.h"

class PhysicsComponent : public Component<PhysicsComponent>
{
public:
    PhysicsComponent(EntityId id = 0) : Component(id) {}
    glm::vec3 velocity;
    glm::vec3 position;
    float mass;
};

#endif // PHYSICSCOMPONENT_H
