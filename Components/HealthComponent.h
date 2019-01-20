#ifndef HEALTHCOMPONENT_H
#define HEALTHCOMPONENT_H

#include "ECS.h"

class HealthComponent : public Component<HealthComponent>
{
public:
    HealthComponent(size_t id = 0, float h = 0.0f) : Component(id), health(h) {}
    float health;
};


#endif // HEALTHCOMPONENT_H
