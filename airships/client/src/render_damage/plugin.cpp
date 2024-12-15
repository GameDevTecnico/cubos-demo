#include "plugin.hpp"
#include "../damageable/plugin.hpp"
#include "../interpolation/plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/glm.hpp>

#include <cubos/engine/render/voxels/plugin.hpp>
#include <cubos/engine/render/voxels/grid.hpp>
#include <cubos/engine/render/voxels/load.hpp>
#include <cubos/engine/transform/plugin.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(airships::client::RenderDamage)
{
    return cubos::core::ecs::TypeBuilder<RenderDamage>("airships::client::RenderDamage")
        .withField("grids", &RenderDamage::grids)
        .withField("offset", &RenderDamage::offset)
        .build();
}

void airships::client::renderDamagePlugin(Cubos& cubos)
{
    cubos.depends(interpolationPlugin);
    cubos.depends(damageablePlugin);
    cubos.depends(renderVoxelsPlugin);
    cubos.depends(transformPlugin);

    cubos.component<RenderDamage>();

    cubos.observer("init render damage").onAdd<RenderDamage>().call([](Commands cmds, Query<Entity> query) {
        for (auto [ent] : query)
        {
            cmds.add(ent, RenderVoxelGrid{});
        }
    });

    cubos.system("render damage")
        .call([](Commands cmds, Query<Entity, const RenderDamage&, RenderVoxelGrid&, const ChildOf&,
                                      const InterpolationOf&, const Damageable&>
                                    query) {
            for (auto [entity, renderDamage, grid, childOf, interpolated, damageable] : query)
            {
                auto it = renderDamage.grids.lower_bound(damageable.health);
                if (it == renderDamage.grids.end())
                {
                    it = renderDamage.grids.begin();
                }

                if (it->second != grid.asset)
                {
                    grid.asset = it->second;
                    grid.offset = renderDamage.offset;
                    cmds.add(entity, LoadRenderVoxels{});
                }
            }
        });
}
