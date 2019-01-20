#ifndef MAGNETICCOMPONENT_H
#define MAGNETICCOMPONENT_H

#include "ECS.h"

class MagneticComponent : public Component<MagneticComponent>
{
public:
    MagneticComponent(EntityId id = 0) : Component(id) {}
    float charge;
};

#endif // MAGNETICCOMPONENT_H
