#ifndef ENTITYMOVED_H
#define ENTITYMOVED_H

#include <glm/vec3.hpp>

#include "ECS.h"

struct Movement {
    EntityId id;
    glm::vec3 oldPosition;
    glm::vec3 newPosition;
};

class EntityMoved : public Event<EntityMoved>
{
public:
    EntityMoved(std::vector<Movement> &_movements) { _movements.swap(movements); }
    std::vector<Movement> movements;
};

#endif // ENTITYMOVED_H
