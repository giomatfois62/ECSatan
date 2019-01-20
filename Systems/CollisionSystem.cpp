#include "CollisionSystem.h"

#include "Events/EntityMoved.h"
#include "Events/Collision.h"
#include "Components/GraphicComponent.h"

CollisionSystem::CollisionSystem()
{
    subscribeTo<EntityMoved>();
    subscribeTo<ItemCreated<PhysicsComponent>>();
}

void CollisionSystem::handleEvent(BaseEvent *event)
{
    const EventType MOVED = EntityMoved::type();
    const EventType CREATED = ItemCreated<PhysicsComponent>::type();

    if(event->getType() == MOVED) {
        EntityMoved *moved = static_cast<EntityMoved*>(event);
        std::vector<std::pair<EntityId, EntityId>> collisions;

        for(struct Movement& movement : moved->movements) {
            //std::cout << "(Collision) Handling move event of component " << movement.id << std::endl;
            if(rand()%5 + 1 == 1)
                collisions.push_back(std::pair<EntityId, EntityId>(movement.id, movement.id + 1));
        }

        if(collisions.size())
            publishEvent(new Collision(collisions));
    }

    if(event->getType() == CREATED) {
        ItemCreated<PhysicsComponent> *created = static_cast<ItemCreated<PhysicsComponent>*>(event);
        PhysicsComponent *c = &(created->item);

        // handle the deleted component
        //std::cout << "(Collision) Handling created event of physics component " << c->id() << std::endl;
    }
}
