#include "PhysicsSystem.h"

#include <iostream>
#include <SDL2/SDL.h>

#include "Components/PhysicsComponent.h"
#include "Components/GraphicComponent.h"
#include "Components/HealthComponent.h"
#include "Components/MagneticComponent.h"
#include "Components/LightComponent.h"

#include "Events/Collision.h"
#include "Events/EntityMoved.h"

using namespace std;

PhysicsSystem::PhysicsSystem()
{
    m_entities = ECS::entitiesWithComponents<GraphicComponent, PhysicsComponent>();
    m_components = ECS::components<PhysicsComponent>();

    subscribeTo<Collision>();
}

void PhysicsSystem::update(float dt)
{
    std::vector<Movement> movements;

    unsigned int time, elapsed;

    time = SDL_GetTicks();
    for(const size_t &entityId : *m_entities) {
        PhysicsComponent *p = ECS::component<PhysicsComponent>(entityId);

        glm::vec3 position = p->position;
        position += dt*p->velocity;

        if(rand()%10 + 1 == 1) {
            struct Movement movement = {entityId, p->position, position};
            movements.push_back(movement);
        }

        p->position = position;
    }

    elapsed = SDL_GetTicks() - time;
    cout << "(Physics) Time to process "<< m_entities->size() << " entities from tree: " << elapsed << "ms" <<endl;

    if(movements.size())
        publishEvent(new EntityMoved(movements));

    int processed = 0;
    time = SDL_GetTicks();
    for(PhysicsComponent &p : *m_components)
    {
        // skip dummy components
        if(!p.isValid())
            continue;

        glm::vec3 position = p.position;
        position += dt*p.velocity;
        p.position = position;

        if(rand()%10 + 1 == 1) {
            struct Movement movement = {p.id(), p.position, position};
            movements.push_back(movement);
        }

        processed++;
    }
    elapsed = SDL_GetTicks() - time;
    cout << "(Physics) Time to process " << processed <<" components: " << elapsed << "ms" <<endl;
}

void PhysicsSystem::handleEvent(BaseEvent *event)
{
    EventType COLLISION = Collision::type();
    if(event->getType() == COLLISION) {
        Collision *collisionEvent = static_cast<Collision*>(event);
        for(std::pair<EntityId, EntityId> &pair : collisionEvent->collisions) {
            //cout << "(Physics) Handling collision event between " << pair.first << " " << pair.second << endl;
        }
    }
}
