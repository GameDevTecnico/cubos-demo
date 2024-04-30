#include "plugin.hpp"
#include "../walker/plugin.hpp"
#include "../tile_map/plugin.hpp"
#include "../object/plugin.hpp"
#include "../holdable/plugin.hpp"
#include "../interaction/plugin.hpp"

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

static int getTopOfStack(Entity entity, Query<Entity, const demo::Holdable&, const ChildOf&> heldObjects,
                         Entity& topEntity)
{
    topEntity = entity;

    int topHeight = 1;
    while (auto match = heldObjects.pin(1, topEntity).first())
    {
        auto [next, holdable, childOf] = *match;
        topEntity = next;
        topHeight++;
    }
    return topHeight;
}

void demo::playerControllerPlugin(Cubos& cubos)
{
    cubos.depends(inputPlugin);
    cubos.depends(transformPlugin);
    cubos.depends(rendererPlugin);
    cubos.depends(walkerPlugin);
    cubos.depends(tileMapPlugin);
    cubos.depends(objectPlugin);
    cubos.depends(interactionPlugin);
    cubos.depends(holdablePlugin);

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
                    auto targetEnt = map.entities[target.y][target.x]; // May be null.

                    // Check if the player is currently holding an object.
                    if (auto match = heldObjects.pin(1, playerEnt).first())
                    {
                        auto [heldEnt, holdable, holdableChildOf] = *match;

                        auto targetHoldableMatch = holdableObjects.at(targetEnt);
                        if (holdable.stackable && targetHoldableMatch)
                        {
                            auto [targetObject, targetHoldable] = *targetHoldableMatch;
                            if (!targetHoldable.stackable)
                            {
                                // The target entity is not stackable, so we cannot stack the held entity with it.
                                continue;
                            }

                            Entity topEnt;
                            if (getTopOfStack(targetEnt, heldObjects, topEnt) >= 3)
                            {
                                // The stack is too high, so we cannot stack the held entity with it.
                                continue;
                            }

                            // Stack the held entity on top of the target entity.
                            cmds.relate(heldEnt, topEnt, ChildOf{});
                            cmds.add(heldEnt, Position{{0.0F, 1.0F, 0.0F}});

                            // Switch back to the normal voxel model.
                            grid.asset = controller.normal;
                        }
                        else if (targetEnt.isNull())
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
                            // Interact with the entity. We add and remove the component immediately, as we simply want
                            // to trigger observers.
                            cmds.add(targetEnt, Interaction{.entity = playerEnt});
                            cmds.remove<Interaction>(targetEnt);
                        }
                    }
                    else if (auto match = holdableObjects.at(targetEnt))
                    {
                        // Pick up holdable at the target position (or top of stack, if stackable).
                        getTopOfStack(targetEnt, heldObjects, targetEnt);
                        auto [object, holdable] = *match;
                        CUBOS_ASSERT((object.size == glm::ivec2{1, 1}),
                                     "Only (1,1)-sized holdable objects are supported");

                        cmds.remove<Object>(targetEnt)
                            .relate(targetEnt, playerEnt, ChildOf{})
                            .add(targetEnt, Position{{0.0F, 8.0F, 0.0F}})
                            .add(targetEnt, Rotation{glm::angleAxis(glm::radians(90.0F), glm::vec3{0.0F, 1.0F, 0.0F})});

                        // Switch back the 'holding' voxel model.
                        grid.asset = controller.holding;
                    }
                    else if (!targetEnt.isNull())
                    {
                        // Interact with the entity. We add and remove the component immediately, as we simply want
                        // to trigger observers.
                        cmds.add(targetEnt, Interaction{.entity = playerEnt});
                        cmds.remove<Interaction>(targetEnt);
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
