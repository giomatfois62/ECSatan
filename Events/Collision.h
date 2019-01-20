#ifndef COLLISION_H
#define COLLISION_H

#include "ECS.h"

typedef std::pair<EntityId, EntityId> CollisionPair;

class Collision : public Event<Collision>
{
public:
    Collision(std::vector<CollisionPair> &_collisions) { _collisions.swap(collisions); }
    std::vector<CollisionPair> collisions;
};

#endif // COLLISION_H
