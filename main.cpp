#include <iostream>

#include "Systems/CollisionSystem.h"
#include "Systems/PhysicsSystem.h"
#include "Systems/RenderingSystem.h"

#include "Components/PhysicsComponent.h"
#include "Components/GraphicComponent.h"
#include "Components/MagneticComponent.h"
#include "Components/HealthComponent.h"
#include "Components/LightComponent.h"

#include "Engine.h"

using namespace std;

int main()
{
    Engine engine;

    printf("Component Types:\n %s(%d)\n %s(%d)\n %s(%d)\n %s(%d)\n %s(%d)\n",
           GraphicComponent::name(), GraphicComponent::type(),
           LightComponent::name(), LightComponent::type(),
           MagneticComponent::name(), MagneticComponent::type(),
           HealthComponent::name(), HealthComponent::type(),
           PhysicsComponent::name(), PhysicsComponent::type());

    printf("System Types:\n %s(%d)\n %s(%d)\n",
           PhysicsSystem::name(), PhysicsSystem::type(),
           RenderingSystem::name(), RenderingSystem::type());

    ECS::createSystem<PhysicsSystem>();
    ECS::createSystem<CollisionSystem>();
    ECS::createSystem<RenderingSystem>();

    boost::container::flat_set<uint> s;
    vector<uint> v;

    // create entities
    uint MAX_COMPONENTS = 100000;
    cout << "Creating " << MAX_COMPONENTS << " entities...\n" << endl;
    for(uint i=0; i<MAX_COMPONENTS; i++)
    {
        size_t id = ECS::createEntity();

        //if(rand()%5+1 == 1)
            ECS::createComponent<GraphicComponent>(id);
        if(rand()%5+1 == 2)
            ECS::createComponent<MagneticComponent>(id);
        if(rand()%5+1 == 3)
            ECS::createComponent<HealthComponent>(id, 5);
        if(rand()%5+1 == 4)
            ECS::createComponent<LightComponent>(id);

        PhysicsComponent *p = ECS::createComponent<PhysicsComponent>(id);
        p->position.x = rand()/static_cast<float>(RAND_MAX);
        p->position.y = rand()/static_cast<float>(RAND_MAX);
        p->velocity.x = rand()/static_cast<float>(RAND_MAX);
        p->velocity.y = rand()/static_cast<float>(RAND_MAX);
        p->mass = rand()/static_cast<float>(RAND_MAX);

        s.insert(i);
        v.push_back(i);
    }

    return engine.run();
}



