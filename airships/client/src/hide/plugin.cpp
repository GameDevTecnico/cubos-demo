#include "plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/engine/render/voxels/plugin.hpp>
#include <cubos/engine/render/voxels/grid.hpp>
#include <cubos/engine/render/voxels/load.hpp>
#include <cubos/engine/transform/plugin.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(airships::client::Hide)
{
    return cubos::core::ecs::TypeBuilder<Hide>("airships::client::Hide")
        .withField("component", &Hide::component)
        .build();
}

void airships::client::hidePlugin(Cubos& cubos)
{
    cubos.depends(renderVoxelsPlugin);
    cubos.depends(transformPlugin);

    cubos.component<Hide>();

    cubos.observer("on add Hide")
        .onAdd<Hide>()
        .call([](Commands cmds, Query<Entity, Hide&, const RenderVoxelGrid&> query,
                 Query<Entity, const ChildOf&, Entity> childOf) {
            for (auto [ent, hide, grid] : query)
            {
                hide.component = grid;
                cmds.remove<RenderVoxelGrid>(ent);

                for (auto [childEnt, r, parentEnt] : childOf.pin(1, ent))
                {
                    cmds.add(childEnt, Hide{});
                }
            }
        });

    cubos.observer("on remove Hide")
        .onRemove<Hide>()
        .call([](Commands cmds, Query<Entity, const Hide&> query, Query<Entity, const ChildOf&, Entity> childOf) {
            for (auto [ent, hide] : query)
            {
                cmds.add<RenderVoxelGrid>(ent, hide.component);

                for (auto [childEnt, r, parentEnt] : childOf.pin(1, ent))
                {
                    cmds.remove<Hide>(childEnt);
                }
            }
        });
}
