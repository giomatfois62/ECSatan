#ifndef RENDERINGSYSTEM_H
#define RENDERINGSYSTEM_H

#include "ECS.h"
#include "Components/GraphicComponent.h"

class RenderingSystem :  public System<RenderingSystem>
{
public:
    RenderingSystem();

    void update(float dt);

private:
    EntitySet *m_entities;
    std::vector<GraphicComponent> *m_components;
};

#endif // RENDERINGSYSTEM_H
