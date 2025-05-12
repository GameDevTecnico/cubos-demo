#include "plugin.hpp"
#include "../walker/plugin.hpp"
#include "../player_controller/plugin.hpp"
#include "../tile_map/plugin.hpp"
#include "../path_finding/plugin.hpp"
#include "../health/plugin.hpp"
#include "../object/plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/tel/logging.hpp>

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
        .withField("item", &ZombieController::item)
        .withField("itemRate", &ZombieController::itemRate)
        .withField("hpPenalization", &ZombieController::hpPenalization)
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
    cubos.depends(objectPlugin);

    cubos.component<ZombieController>();

    cubos.system("do ZombieController")
        .call([](Commands cmds, const DeltaTime& dt,
                 Query<Entity, ZombieController&, Walker&, const Health&, Opt<const PathTask&>, Opt<Path&>, ChildOf&,
                       TileMap&, Entity>
                     zombies,
                 Query<Entity, PlayerController&, Walker&> players, Query<Health&, Opt<const Target&>> healths) {
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

                // Check if there's a target entity adjacent to the zombie. In that case, attack it.
                bool attacked = false;
                for (auto direction : {glm::ivec2{0, 1}, glm::ivec2{1, 0}, glm::ivec2{0, -1}, glm::ivec2{-1, 0}})
                {
                    auto aheadPosition = walker.position + direction;
                    if (aheadPosition.x < 0 || aheadPosition.y < 0 || aheadPosition.x >= map.entities.size() ||
                        aheadPosition.y >= map.entities.size())
                    {
                        continue;
                    }

                    auto entityAhead = map.entities[aheadPosition.y][aheadPosition.x];
                    if (auto match = healths.at(entityAhead))
                    {
                        // If the entity ahead is damageable, is a target, and isn't in our team, attack it.
                        auto [health, target] = *match;
                        if (health.team != zombieHealth.team && target && controller.hitTimeAcc >= controller.hitTime)
                        {
                            walker.direction = direction;
                            controller.hitTimeAcc = 0.0F;
                            health.hp -= 1;
                            attacked = true;
                            break;
                        }
                    }
                }

                // We are attacking a target, no need to move.
                if (attacked)
                {
                    continue;
                }

                if (pathTask)
                {
                    // If there's anything adjacent, just attack it.
                    for (auto direction : {glm::ivec2{0, 1}, glm::ivec2{1, 0}, glm::ivec2{0, -1}, glm::ivec2{-1, 0}})
                    {
                        auto aheadPosition = walker.position + direction;
                        if (aheadPosition.x < 0 || aheadPosition.y < 0 || aheadPosition.x >= map.entities.size() ||
                            aheadPosition.y >= map.entities.size())
                        {
                            continue;
                        }

                        auto entityAhead = map.entities[aheadPosition.y][aheadPosition.x];
                        if (auto match = healths.at(entityAhead))
                        {
                            // If the entity ahead is damageable, is a target, and isn't in our team, attack it.
                            auto [health, target] = *match;
                            if (health.team != zombieHealth.team && controller.hitTimeAcc >= controller.hitTime)
                            {
                                walker.facing = direction;
                                controller.hitTimeAcc = 0.0F;
                                health.hp -= 1;
                                attacked = true;
                                break;
                            }
                        }
                    }

                    continue;
                }

                if (!path || path->path.empty() || controller.rePathTimeAcc >= controller.rePathTime)
                {
                    controller.rePathTimeAcc = 0.0F;

                    // We don't have a path, plan a new one.
                    cmds.add(zombieEnt, PathTask{
                                            .from = walker.position,
                                            .team = zombieHealth.team,
                                            .hpPenalization = controller.hpPenalization,
                                        });

                    continue;
                }

                controller.rePathTimeAcc += dt.value();

                // We have a non-empty path, so we'll just follow it.
                if (path->path.back() == walker.position)
                {
                    path->path.pop_back();
                }

                if (path->path.empty())
                {
                    continue;
                }

                auto next = path->path.back();
                walker.direction = next - walker.position;

                if (auto match = healths.at(map.entities[next.y][next.x]))
                {
                    auto [health, _2] = *match;

                    if (health.team != zombieHealth.team)
                    {
                        if (controller.hitTimeAcc >= controller.hitTime)
                        {
                            controller.hitTimeAcc = 0.0F;
                            health.hp -= 1;
                        }

                        continue;
                    }
                }

                if (!map.entities[next.y][next.x].isNull())
                {
                    // The path is invalid, try planning a new one.
                    path->path.clear();
                }
            }
        });

    cubos.observer("drop item on Zombie death")
        .onRemove<ZombieController>()
        .call([](Commands cmds, Assets& assets,
                 Query<const Walker&, const ZombieController&, const ChildOf&, Entity> query) {
            for (auto [walker, controller, _, mapEnt] : query)
            {
                if (std::rand() / (float)RAND_MAX < controller.itemRate)
                {
                    auto dropEnt = cmds.spawn(assets.read(controller.item)->blueprint()).entity();
                    cmds.relate(dropEnt, mapEnt, ChildOf{});
                    cmds.add(dropEnt, Object{.position = walker.position, .size = {1, 1}});
                }
            }
        });
}
