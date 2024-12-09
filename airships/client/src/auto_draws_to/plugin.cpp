#include "plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/render/camera/plugin.hpp>
#include <cubos/engine/render/camera/draws_to.hpp>
#include <cubos/engine/render/target/plugin.hpp>
#include <cubos/engine/render/target/target.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(airships::client::AutoDrawsTo)
{
    return cubos::core::ecs::TypeBuilder<AutoDrawsTo>("airships::client::AutoDrawsTo").build();
}

void airships::client::autoDrawsToPlugin(Cubos& cubos)
{
    cubos.depends(renderTargetPlugin);
    cubos.depends(cameraPlugin);

    cubos.component<AutoDrawsTo>();

    cubos.system("auto add DrawsTo to render targets")
        .call([](Commands cmds, Query<Entity, const AutoDrawsTo&> query, Query<Entity, const RenderTarget&> targets) {
            for (auto [entity, autoDrawsTo] : query)
            {
                for (auto [targetEntity, target] : targets)
                {
                    cmds.relate(entity, targetEntity, DrawsTo{});
                    cmds.remove<AutoDrawsTo>(entity);
                    break;
                }
            }
        });
}
