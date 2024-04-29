#include "plugin.hpp"
#include "../walker/plugin.hpp"
#include "../player_controller/plugin.hpp"
#include "cubos/engine/assets/plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>

#include <cubos/engine/input/plugin.hpp>
#include <cubos/engine/scene/scene.hpp>
#include <cubos/engine/assets/asset.hpp>
#include <cubos/engine/assets/assets.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(demo::ZombieController)
{
    return cubos::core::ecs::TypeBuilder<ZombieController>("demo::ZombieController")
        .withField("moveX", &ZombieController::moveX)
        .withField("moveY", &ZombieController::moveY)
        .withField("normal", &ZombieController::normal)
        .build();
}

void demo::zombiePlugin(Cubos& cubos)
{
    cubos.depends(playerControllerPlugin);
    cubos.depends(walkerPlugin);
    cubos.depends(assetsPlugin);
    static Asset<Scene> ZombieAsset = AnyAsset("4d8ff909-46df-49c4-adf0-ca409b34510f");

    cubos.component<ZombieController>();

    cubos.startupSystem("Initialize zombie").tagged(assetsTag).call([](Commands cmds, const Assets& assets) {
        auto entity = cmds.spawn(assets.read(ZombieAsset)->blueprint).entity("base.body");
        cmds.add(entity, Walker{});
    });

    /** cubos.system("do ZombieController").call([](Query<ZombieController&, Walker&> query) {
         for (auto [controller, walker] : query)
         {
             if (walker.direction != glm::ivec2{0, 0})
             {
                 // The entity is already moving, so we can skip it.
                 continue;
             }

             walker.direction.x = static_cast<int>(glm::round(input.axis(controller.moveX.c_str(), controller.player)));
             if (walker.direction.x == 0)
             {
                 walker.direction.y =
                     static_cast<int>(glm::round(input.axis(controller.moveY.c_str(), controller.player)));
             }
         }
     });*/
}
