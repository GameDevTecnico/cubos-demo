#include "plugin.hpp"

#include "../destroy_tree/plugin.hpp"
#include "../player_controller/plugin.hpp"
#include "../health/plugin.hpp"
#include "../level/plugin.hpp"
#include "../scoreboard/plugin.hpp"

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
    cubos.depends(playerControllerPlugin);
    cubos.depends(healthPlugin);
    cubos.depends(levelPlugin);
    cubos.depends(scoreboardPlugin);

    cubos.resource<Restart>();

    cubos.system("restart the scene when only one player remains")
        .call([](Commands cmds, Scoreboard& score, Restart& restart, const DeltaTime& dt, Assets& assets, Query<Entity, const PlayerController&, const Health&> query, Query<Entity, const Level&> queryLevel) {
            // Store reference to a player; we only want a single reference,
            // so we quit if we find another valid one
            const PlayerController* playerPtr = nullptr;
            for(auto [ent, player, health] : query) {
                if( health.hp > 0 ) {
                    if(!playerPtr) {
                        playerPtr = &player;
                    } else {
                        // Quit
                        return;
                    }
                }
            }

            // We have win condition!
            restart.accumT += dt.value();
            if(restart.accumT >= 3) {
                restart.accumT = 0;
                for(auto [ent, level] : queryLevel) {
                    CUBOS_INFO("Found it! {}", ent);
                    CUBOS_ERROR("Entity {} will commit die", ent);
                    cmds.add(ent, DestroyTree{});

                    cmds.spawn(*assets.read(MainSceneAsset)).named("main");
                }
            }
        });
}