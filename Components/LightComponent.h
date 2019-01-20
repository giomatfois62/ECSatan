#ifndef LIGHTCOMPONENT_H
#define LIGHTCOMPONENT_H

#include "ECS.h"

#include <glm/vec4.hpp>

class LightComponent : public Component<LightComponent>
{
public:
    LightComponent(EntityId id = 0) : Component(id) {}
    float intensity;
    glm::vec4 direction;
};

#endif // LIGHTCOMPONENT_H
