#ifndef GRAPHICCOMPONENT_H
#define GRAPHICCOMPONENT_H

#include <glm/mat4x4.hpp>

#include "ECS.h"

class GraphicComponent : public Component<GraphicComponent>
{
public:
    GraphicComponent(EntityId id = 0) : Component(id) {}
    glm::mat4 transform;
};

#endif // GRAPHICCOMPONENT_H
