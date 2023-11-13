#include "plugin.hpp"

#include <cubos/engine/collisions/collider.hpp>
#include <cubos/engine/transform/local_to_world.hpp>
#include <cubos/engine/gizmos/plugin.hpp>

using cubos::core::ecs::Query;
using cubos::core::ecs::Read;
using cubos::core::ecs::Write;

using namespace cubos::engine;

static void gizmosSystem(Write<Gizmos> gizmos, Query<Read<LocalToWorld>, Read<Collider>> colliders)
{
    for (auto [entity, transform, collider] : colliders)
    {
        gizmos->drawWireBox("collider", collider->worldAABB.diag[0], collider->worldAABB.diag[1]);
    }
}

void demo::colliderGizmosPlugin(Cubos& cubos)
{
    cubos.addPlugin(gizmosPlugin);
    cubos.system(gizmosSystem).before("cubos.gizmos.draw").after("cubos.collisions.aabb.update");
}
