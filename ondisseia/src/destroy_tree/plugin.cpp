#include "plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/transform/plugin.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(demo::DestroyTree)
{
    return cubos::core::ecs::TypeBuilder<DestroyTree>("demo::DestroyTree").build();
}

void demo::destroyTreePlugin(Cubos& cubos)
{
    cubos.component<DestroyTree>();

    cubos.depends(transformPlugin);

    cubos.observer("on DestroyTree")
        .onAdd<DestroyTree>()
        .call([](Commands cmds, Query<Entity> query, Query<Entity, const ChildOf&> children) {
            for (auto [entity] : query)
            {
                for (auto [child, childOf] : children.pin(1, entity))
                {
                    cmds.add(child, DestroyTree{});
                }

                cmds.destroy(entity);
            }
        });
}
