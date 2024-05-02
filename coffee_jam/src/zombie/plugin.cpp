#include "plugin.hpp"
#include "../walker/plugin.hpp"
#include "../player_controller/plugin.hpp"
#include "../tile_map/plugin.hpp"
#include "../path_finding/plugin.hpp"
#include "../health/plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/log.hpp>

#include <cubos/engine/input/plugin.hpp>
#include <cubos/engine/scene/scene.hpp>
#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/fixed_step/plugin.hpp>
#include <glm/ext/vector_int2.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(demo::ZombieController)
{
    return cubos::core::ecs::TypeBuilder<ZombieController>("demo::ZombieController")
        .withField("rePathTime", &ZombieController::rePathTime)
        .withField("rePathTimeAcc", &ZombieController::rePathTimeAcc)
        .withField("hitTime", &ZombieController::hitTime)
        .withField("hitTimeAcc", &ZombieController::hitTimeAcc)
        .withField("lastPosition", &ZombieController::lastPosition)
        .build();
}

void demo::zombiePlugin(Cubos& cubos)
{
    cubos.depends(playerControllerPlugin);
    cubos.depends(walkerPlugin);
    cubos.depends(tileMapPlugin);
    cubos.depends(assetsPlugin);
    cubos.depends(transformPlugin);
    cubos.depends(pathFindingPlugin);
    cubos.depends(healthPlugin);

    cubos.component<ZombieController>();

    cubos.system("do ZombieController")
        .call([](Commands cmds, const DeltaTime& dt,
                 Query<Entity, ZombieController&, Walker&, const Health&, Opt<const PathTask&>, Opt<Path&>, ChildOf&,
                       TileMap&, Entity>
                     zombies,
                 Query<Entity, PlayerController&, Walker&> players, Query<Health&> healths) {
            for (auto [zombieEnt, controller, walker, zombieHealth, pathTask, path, _1, map, mapEnt] : zombies)
            {
                if (controller.hitTimeAcc < controller.hitTime)
                {
                    controller.hitTimeAcc += dt.value();
                }

                if (walker.direction != glm::ivec2{0, 0} || !walker.initialized)
                {
                    // The entity is already moving, so we can skip it.
                    continue;
                }

                // If we're calculating a path, then wait for the answer before moving.
                if (pathTask)
                {
                    continue;
                }

                // Find the player which is closest to the zombie.
                Entity targetPlayer{};
                glm::ivec2 targetPosition{};
                for (auto [playerEnt, _2, playerWalker] : players)
                {
                    if (targetPlayer.isNull() ||
                        glm::distance((glm::vec2)walker.position, (glm::vec2)targetPosition) >
                            glm::distance((glm::vec2)walker.position, (glm::vec2)playerWalker.position))
                    {
                        targetPlayer = playerEnt;
                        targetPosition = playerWalker.position;
                    }
                }

                // If there isn't a target player, just do nothing.
                if (targetPlayer.isNull())
                {
                    continue;
                }

                if (glm::abs(walker.position.x - targetPosition.x) + glm::abs(walker.position.y - targetPosition.y) <=
                    1)
                {
                    // If there's a player adjacent to the zombie, just attack them.
                    if (controller.hitTimeAcc >= controller.hitTime)
                    {
                        controller.hitTimeAcc = 0.0F;
                        auto [health] = *healths.at(targetPlayer);
                        health.hp -= 1;
                    }

                    continue;
                }

                if (path)
                {
                    if (path->path.empty())
                    {
                        // We asked for a path, but there isn't one. We'll just move in the direction of the closest
                        // player. Eventually, we'll ask for a new path.
                        controller.rePathTimeAcc += dt.value();
                        if (controller.rePathTimeAcc > controller.rePathTime)
                        {
                            controller.rePathTimeAcc = 0.0F;
                            cmds.remove<Path>(zombieEnt);
                        }
                    }
                    else
                    {
                        // We have a non-empty path, so we'll just follow it.
                        if (path->path.back() == walker.position)
                        {
                            path->path.pop_back();
                            if (path->path.empty())
                            {
                                cmds.remove<Path>(zombieEnt);
                                continue;
                            }

                            walker.direction = path->path.back() - walker.position;
                            continue;
                        }

                        // We're stuck, discard the path, and just move in the direction of the closest player.
                        cmds.remove<Path>(zombieEnt);
                    }
                }

                if (controller.lastPosition == walker.position && !path)
                {
                    // The previous movement failed, and we do not have a pth.
                    // Generate a new path to the closest player.
                    controller.lastPosition = {-1, -1};
                    cmds.add(zombieEnt, PathTask{walker.position, targetPosition});
                }
                else
                {
                    controller.lastPosition = walker.position;

                    // Just move in the direction of the closest player.
                    bool directionPreference = std::rand() % 2 == 0;

                    if (targetPosition.x == walker.position.x ||
                        (directionPreference && targetPosition.y != walker.position.y))
                    {
                        if (targetPosition.y < walker.position.y)
                        {
                            walker.direction = {0, -1};
                        }
                        else
                        {
                            walker.direction = {0, 1};
                        }
                    }
                    else
                    {
                        if (targetPosition.x < walker.position.x)
                        {
                            walker.direction = {-1, 0};
                        }
                        else
                        {
                            walker.direction = {1, 0};
                        }
                    }

                    auto entityAhead =
                        map.entities[walker.position.y + walker.direction.y][walker.position.x + walker.direction.x];
                    if (auto match = healths.at(entityAhead))
                    {
                        // If the entity ahead is damageable, and isn't in our team, attack it.
                        auto [health] = *match;
                        if (health.team != zombieHealth.team && controller.hitTimeAcc >= controller.hitTime)
                        {
                            controller.hitTimeAcc = 0.0F;
                            health.hp -= 1;
                        }
                    }
                }
            }
        });
}
