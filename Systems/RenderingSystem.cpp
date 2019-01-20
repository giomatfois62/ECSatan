#include "RenderingSystem.h"

#include <SDL2/SDL.h>
#include <iostream>

#include "Events/Collision.h"
#include "Events/EntityMoved.h"
#include "Components/PhysicsComponent.h"

using namespace std;

RenderingSystem::RenderingSystem()
{
    m_entities = ECS::entitiesWithComponents<GraphicComponent, PhysicsComponent>();
    m_components = ECS::components<GraphicComponent>();
}

void RenderingSystem::update(float dt)
{
    unsigned int time, elapsed;
    int processed = 0;

//    vector<EntityId> tmp;
//    tmp.insert(tmp.end(), m_entities->begin(), m_entities->end());

    time = SDL_GetTicks();
    //for(EntitySet::iterator it = m_entities->begin(); it != m_entities->end(); ++it) {
    for(const EntityId &id : *m_entities) {
    //for(size_t id : tmp) {
        PhysicsComponent *p = ECS::component<PhysicsComponent>(id);
        GraphicComponent *g = ECS::component<GraphicComponent>(id);
        // draw

        processed++;
    }
    elapsed = SDL_GetTicks() - time;
    cout << "(Rendering) Time to process "<< processed << " entities from tree: " << elapsed << "ms" <<endl;

    processed = 0;
    time = SDL_GetTicks();
    for(GraphicComponent &g : *m_components)
    {
        //PhysicsComponent *p = ECS::component<PhysicsComponent>(g.id());
        if(g.isValid()) {
            // draw
            processed++;
        }
    }
    elapsed = SDL_GetTicks() - time;
    cout << "(Rendering) Time to process " << processed <<" components: " << elapsed << "ms" <<endl;

    processed = 0;
    int entities = 0;
    time = SDL_GetTicks();
    for(ComponentList &list : ECS::entities().items()) {
        PhysicsComponent *p = ECS::component<PhysicsComponent>(processed);
        if(p->isValid()) {
            GraphicComponent *g = ECS::component<GraphicComponent>(processed);
            if(g->isValid())
                entities++;
        }
        processed++;
    }
    elapsed = SDL_GetTicks() - time;
    cout << "(Rendering) Time to process " << entities <<" entities: " << elapsed << "ms" <<endl;
}
