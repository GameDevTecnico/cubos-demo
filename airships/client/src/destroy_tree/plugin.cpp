#include "plugin.hpp"
#include "../interpolation/plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/transform/plugin.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(airships::client::DestroyTree)
{
    return cubos::core::ecs::TypeBuilder<DestroyTree>("airships::client::DestroyTree").build();
}

void airships::client::destroyTreePlugin(Cubos& cubos)
{
    cubos.component<DestroyTree>();

    cubos.depends(transformPlugin);
    cubos.depends(interpolationPlugin);

    cubos.observer("on DestroyTree")
        .onAdd<DestroyTree>()
        .call([](Commands cmds, Query<Entity> query, Query<Entity, const ChildOf&> children,
                 Query<Entity, const InterpolationOf&> interpolations) {
            for (auto [entity] : query)
            {
                for (auto [child, childOf] : children.pin(1, entity))
                {
                    cmds.add(child, DestroyTree{});
                }

                for (auto [interpolation, interpolationOf] : interpolations.pin(1, entity))
                {
                    cmds.add(interpolation, DestroyTree{});
                }

                cmds.destroy(entity);
            }
        });
}
