#include "plugin.hpp"
#include "../player_movement/plugin.hpp"
#include "../tile_map/plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>

#include <cubos/engine/input/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/render/voxels/grid.hpp>
#include <cubos/engine/render/voxels/load.hpp>
#include <cubos/engine/render/voxels/plugin.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(demo::PlayerController)
{
    return cubos::core::ecs::TypeBuilder<PlayerController>("demo::PlayerController")
        .withField("player", &PlayerController::player)
        .withField("moveX", &PlayerController::moveX)
        .withField("moveY", &PlayerController::moveY)
        .withField("model", &PlayerController::model)
        .build();
}


void demo::playerControllerPlugin(Cubos& cubos)
{
    cubos.depends(inputPlugin);
    cubos.depends(transformPlugin);
    cubos.depends(renderVoxelsPlugin);
    cubos.depends(movementPlugin);
    cubos.depends(tileMapPlugin);

    cubos.component<PlayerController>();

    cubos.system("player controller handler")
        .after(inputUpdateTag)
        .call([](Commands cmds, Input& input,
                 Query<Entity, RenderVoxelGrid&, PlayerController&, Movement&> players) {

            for (auto [playerEnt, grid, controller, movement] : players)
            {
                // Load the player model if not loaded.
                if (grid.asset != controller.model)
                {
                    grid.asset = controller.model;
                    cmds.add(playerEnt, LoadRenderVoxels{});
                }

                // If the player is already moving then skip.
                if (movement.direction != glm::ivec2{0, 0})
                {
                    continue;
                }

                // Move the character as requested.
                auto moveX = input.axis(controller.moveX.c_str(), controller.player);
                auto moveY = -input.axis(controller.moveY.c_str(), controller.player);

                // Handle input deadzones.
                if (glm::abs(moveX) < 0.75F)
                {
                    moveX = 0.0F;
                }
                if (glm::abs(moveY) < 0.75F)
                {
                    moveY = 0.0F;
                }

                movement.direction.x = static_cast<int>(glm::round(moveX));
                movement.direction.y = static_cast<int>(glm::round(moveY));

                // Only allow moving in one axis.
                if (movement.direction.x != 0)
                {
                    movement.direction.y = 0;
                }
            }
        });

    /*cubos.observer("drop item on player death")*/
    /*    .onRemove<PlayerController>(1)*/
    /*    .call([](Commands cmds,*/
    /*             Query<Entity, const Holdable&, const ChildOf&, const Walker&, const ChildOf&, Entity> holdables) {*/
    /*        for (auto [itemEnt, holdable, _1, walker, _2, mapEnt] : holdables)*/
    /*        {*/
    /*            cmds.relate(itemEnt, mapEnt, ChildOf{});*/
    /*            cmds.add(itemEnt, Object{.position = walker.position, .size = {1, 1}});*/
    /*        }*/
    /*    });*/
}
