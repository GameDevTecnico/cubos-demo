#include "plugin.hpp"
#include "offset.hpp"

#include <cubos/engine/transform/local_to_world.hpp>

#include <glm/gtc/matrix_transform.hpp>

using cubos::core::ecs::Query;
using cubos::core::ecs::Read;
using cubos::core::ecs::Write;

using namespace cubos::engine;

using namespace demo;

static void offsetSystem(Query<Read<Offset>> offsets, Query<Write<LocalToWorld>> localToWorlds)
{
    for (auto [entity, offset] : offsets)
    {
        auto [entityLocalToWorld] = *localToWorlds[entity];
        auto [parentLocalToWorld] = *localToWorlds[offset->parent];
        entityLocalToWorld->mat = parentLocalToWorld->mat * glm::translate(glm::mat4(1.0f), offset->vec);
    }
}

void demo::offsetPlugin(Cubos& cubos)
{
    cubos.addComponent<Offset>();
    cubos.system(offsetSystem).after("cubos.transform.update").before("cubos.renderer.frame");
}
