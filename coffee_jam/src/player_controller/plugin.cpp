#include "plugin.hpp"
#include "../walker/plugin.hpp"
#include "../tile_map/plugin.hpp"
#include "../object/plugin.hpp"
#include "../holdable/plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>

#include <cubos/engine/input/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/renderer/plugin.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(demo::PlayerController)
{
    return cubos::core::ecs::TypeBuilder<PlayerController>("demo::PlayerController")
        .withField("player", &PlayerController::player)
        .withField("moveX", &PlayerController::moveX)
        .withField("moveY", &PlayerController::moveY)
        .withField("normal", &PlayerController::normal)
        .withField("holding", &PlayerController::holding)
        .build();
}

void demo::playerControllerPlugin(Cubos& cubos)
{
    cubos.depends(inputPlugin);
    cubos.depends(transformPlugin);
    cubos.depends(walkerPlugin);
    cubos.depends(tileMapPlugin);
    cubos.depends(objectPlugin);
    cubos.depends(holdablePlugin);
    cubos.depends(rendererPlugin);

    cubos.component<PlayerController>();

    cubos.system("do PlayerController")
        .after(inputUpdateTag)
        .call([](Commands cmds, Input& input,
                 Query<Entity, RenderableGrid&, PlayerController&, Walker&, const ChildOf&, TileMap&, Entity> players,
                 Query<const Object&, const Holdable&> holdableObjects,
                 Query<Entity, const Holdable&, const ChildOf&> heldObjects) {
            for (auto [playerEnt, grid, controller, walker, childOf, map, mapEnt] : players)
            {
                if (walker.direction != glm::ivec2{0, 0})
                {
                    // The entity is already moving, so we can skip it.
                    continue;
                }

                if (grid.asset.isNull())
                {
                    grid.asset = controller.normal;
                }

                if (input.justPressed(controller.interact.c_str(), controller.player))
                {
                    auto target = walker.position + walker.facing;
                    if (target.x < 0 || target.x >= map.entities.size() || target.y < 0 ||
                        target.y >= map.entities.size())
                    {
                        // The position the player is facing is out of bounds.
                        continue;
                    }
                    auto targetEntity = map.entities[target.y][target.x]; // May be null.

                    // Check if the player is currently holding an object.
                    if (auto match = heldObjects.pin(1, playerEnt).first())
                    {
                        auto [heldEnt, holdable, holdableChildOf] = *match;

                        if (targetEntity.isNull())
                        {
                            // Place the object in the target position, as it is empty.
                            map.entities[target.y][target.x] = heldEnt;
                            cmds.relate(heldEnt, mapEnt, ChildOf{});
                            cmds.add(heldEnt, Object{.position = target, .size = {1, 1}});

                            // Switch back to the normal voxel model.
                            grid.asset = controller.normal;
                        }
                        else
                        {
                            // TODO: add tool interactions? i.e. recharging turrets, construction, etc
                        }
                    }
                    else if (auto match = holdableObjects.at(targetEntity))
                    {
                        // Pick up object at the target position.
                        auto [object, holdable] = *match;
                        CUBOS_ASSERT((object.size == glm::ivec2{1, 1}),
                                     "Only (1,1)-sized holdable objects are supported");

                        cmds.remove<Object>(targetEntity);
                        cmds.relate(targetEntity, playerEnt, ChildOf{});
                        cmds.add(targetEntity, Position{{0.0F, 8.0F, 0.0F}});

                        // Switch back the 'holding' voxel model.
                        grid.asset = controller.holding;
                    }
                }
                else
                {
                    // Move the character as requested.
                    walker.direction.x =
                        static_cast<int>(glm::round(input.axis(controller.moveX.c_str(), controller.player)));
                    if (walker.direction.x == 0)
                    {
                        walker.direction.y =
                            static_cast<int>(glm::round(input.axis(controller.moveY.c_str(), controller.player)));
                    }
                }
            }
        });
}
