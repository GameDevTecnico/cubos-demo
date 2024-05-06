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

                if (heldObjects.pin(1, playerEnt).empty())
                {
                    grid.asset = controller.normal;
                }
                else
                {
                    grid.asset = controller.holding;
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

                            // Stack the held entity on top of the target entity with a random rotation.
                            auto rotation = ((float)rand() / (float)RAND_MAX) * 180.0F;
                            cmds.relate(heldEnt, topEnt, ChildOf{});
                            cmds.add(heldEnt, Position{{0.0F, 1.0F, 0.0F}})
                                .add(heldEnt,
                                     Rotation{glm::angleAxis(glm::radians(rotation), glm::vec3{0.0F, 1.0F, 0.0F})});
                        }
                        else if (targetEnt.isNull())
                        {
                            // Place the object in the target position, as it is empty.
                            map.entities[target.y][target.x] = heldEnt;
                            cmds.relate(heldEnt, mapEnt, ChildOf{});
                            cmds.add(heldEnt, Object{.position = target, .size = {1, 1}});
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
                            .add(targetEnt, Rotation{});
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
                    auto moveX = input.axis(controller.moveX.c_str(), controller.player);
                    auto moveY = -input.axis(controller.moveY.c_str(), controller.player);
                    if (glm::abs(moveX) < 0.75F)
                    {
                        moveX = 0.0F; // Deadzone.
                    }
                    if (glm::abs(moveY) < 0.75F)
                    {
                        moveY = 0.0F; // Deadzone.
                    }

                    walker.direction.x = static_cast<int>(glm::round(moveX));
                    if (walker.direction.x == 0)
                    {
                        walker.direction.y = static_cast<int>(glm::round(moveY));
                    }
                }
            }
        });

    cubos.observer("drop item on player death")
        .onRemove<PlayerController>(1)
        .call([](Commands cmds,
                 Query<Entity, const Holdable&, const ChildOf&, const Walker&, const ChildOf&, Entity> holdables) {
            for (auto [itemEnt, holdable, _1, walker, _2, mapEnt] : holdables)
            {
                cmds.relate(itemEnt, mapEnt, ChildOf{});
                cmds.add(itemEnt, Object{.position = walker.position, .size = {1, 1}});
            }
        });
}
