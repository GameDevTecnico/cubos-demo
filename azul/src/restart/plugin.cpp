#include "plugin.hpp"

#include "../destroy_tree/plugin.hpp"

#include <cubos/core/reflection/external/vector.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/ecs/reflection.hpp>

#include <cubos/core/ecs/name.hpp>
#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/scene/scene.hpp>

using namespace cubos::engine;

static const Asset<Scene> MainSceneAsset = AnyAsset("/assets/scenes/main.cubos");

CUBOS_REFLECT_IMPL(demo::Restart)
{
    return cubos::core::ecs::TypeBuilder<Restart>("demo::Restart")
        .build();
}

void demo::restartPlugin(Cubos& cubos)
{
    cubos.depends(assetsPlugin);

    cubos.component<Restart>();

    cubos.observer("restart the scene whenver the component is present")
        .onAdd<Restart>()
        .call([](Assets& assets, Commands cmds, Query<Entity> _1, Query<Entity, const cubos::core::ecs::Name&> query) {
            // Remove all entities
            for(auto [ent, name] : query) {
                CUBOS_INFO("Found it! {}", name);
                if(name.value == "main") {
                    CUBOS_INFO("Found it! {}", ent);
                    CUBOS_ERROR("Entity {} will commit die", ent);
                    cmds.add(ent, DestroyTree{});

                    cmds.spawn(*assets.read(MainSceneAsset)).named("main");
                }
            }
        });
}