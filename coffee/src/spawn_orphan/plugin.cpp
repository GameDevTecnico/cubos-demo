#include "plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/render/target/plugin.hpp>
#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/transform/child_of.hpp>
#include <cubos/engine/transform/local_to_world.hpp>
#include <cubos/engine/transform/position.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(coffee::SpawnOrphan)
{
    return cubos::core::ecs::TypeBuilder<SpawnOrphan>("coffee::SpawnOrphan").wrap(&SpawnOrphan::scene);
}

void coffee::spawnOrphanPlugin(Cubos& cubos)
{
    cubos.depends(transformPlugin);
    cubos.depends(renderTargetPlugin);
    cubos.depends(assetsPlugin);

    cubos.component<SpawnOrphan>();

    cubos.system("spawn orphan")
        .after(transformUpdateTag)
        .after(drawToRenderTargetTag)
        .call([](Commands commands, Assets& assets,
                 Query<Entity, const SpawnOrphan&, const LocalToWorld&, Position&, const ChildOf&, Entity> query) {
            for (auto [ent1, spawnOrphan, localToWorld, pos, childOf, ent2] : query)
            {
                commands.destroy(ent1);
                commands.spawn(assets.read(spawnOrphan.scene)->blueprint())
                    .add(Position{localToWorld.worldPosition()})
                    .named("orphan");
            }
        });
}