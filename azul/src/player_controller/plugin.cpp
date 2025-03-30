#include "plugin.hpp"
#include "../player_movement/plugin.hpp"
#include "../tile_map/plugin.hpp"
#include "../bullet/plugin.hpp"
#include "../health/plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>

#include <cubos/engine/input/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/scene/plugin.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(demo::PlayerController)
{
    return cubos::core::ecs::TypeBuilder<PlayerController>("demo::PlayerController")
        .withField("player", &PlayerController::player)
        .withField("moveX", &PlayerController::moveX)
        .withField("moveY", &PlayerController::moveY)
        .withField("shoot", &PlayerController::shoot)
        .withField("bullet", &PlayerController::bullet)
        .withField("bulletReloadTime", &PlayerController::bulletReloadTime)
        .withField("bulletSpeed", &PlayerController::bulletSpeed)
        .withField("shootMinDistance", &PlayerController::shootMinDistance)
        .withField("shootConeWidenDistance", &PlayerController::shootConeWidenDistance)
        .withField("bulletReloadAcc", &PlayerController::bulletReloadAcc)
        .build();
}

void demo::playerControllerPlugin(Cubos& cubos)
{
    cubos.depends(inputPlugin);
    cubos.depends(transformPlugin);
    cubos.depends(scenePlugin);
    cubos.depends(movementPlugin);
    cubos.depends(tileMapPlugin);
    cubos.depends(bulletPlugin);
    cubos.depends(assetsPlugin);
    cubos.depends(healthPlugin);

    cubos.component<PlayerController>();

    cubos.system("player controller handler")
        .after(inputUpdateTag)
        .call([](Commands cmds, Input& input, const DeltaTime& dt,
                 Query<const Position&, PlayerController&, Movement&, Health&, const ChildOf&, const TileMap&, Entity>
                     players,
                 Assets& assets) {
            for (auto [position, controller, movement, health, _1, tileMap, tileMapEnt] : players)
            {
                // If the player is already moving then skip.
                if (movement.direction != glm::ivec2{0, 0})
                {
                    continue;
                }

                // Move the character as requested.
                auto moveX = -input.axis(controller.moveX.c_str(), controller.player);
                auto moveY = -input.axis(controller.moveY.c_str(), controller.player);
                if (health.hp <= 0)
                {
                    moveX = 0;
                    moveY = 0;
                }

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

                if (controller.bulletReloadAcc > 0.0F)
                {
                    controller.bulletReloadAcc -= dt.value();
                }

                // Shoot if requested.
                if (health.hp > 0 && input.justPressed(controller.shoot.c_str(), controller.player) &&
                    movement.facing != glm::ivec2{0, 0} && controller.bulletReloadAcc <= 0.0F)
                {
                    controller.bulletReloadAcc = controller.bulletReloadTime;

                    // Find entities to the left or right of the boat.
                    std::vector<glm::ivec2> targets = {};
                    size_t sideAxis = movement.facing.x == 0 ? 0 : 1;
                    size_t frontAxis = 1 - sideAxis;
                    for (int u = movement.position[sideAxis] - 1; u >= 0; --u)
                    {
                        int coneSide = glm::abs(movement.position[sideAxis] - u) / controller.shootConeWidenDistance;
                        for (int v = glm::max(0, movement.position[frontAxis] - coneSide);
                             v <= glm::min((int)tileMap.tiles.size() - 1, movement.position[frontAxis] + coneSide); ++v)
                        {
                            glm::ivec2 pos{};
                            pos[sideAxis] = u;
                            pos[frontAxis] = v;
                            if (!tileMap.entities[pos.y][pos.x].isNull())
                            {
                                targets.push_back(pos);
                                break;
                            }
                        }
                    }
                    for (int u = movement.position[sideAxis] + 1; u < tileMap.tiles.size(); ++u)
                    {
                        int coneSide = glm::abs(movement.position[sideAxis] - u) / controller.shootConeWidenDistance;
                        for (int v = glm::max(0, movement.position[frontAxis] - coneSide);
                             v <= glm::min((int)tileMap.tiles.size() - 1, movement.position[frontAxis] + coneSide); ++v)
                        {
                            glm::ivec2 pos{};
                            pos[sideAxis] = u;
                            pos[frontAxis] = v;

                            if (!tileMap.entities[pos.y][pos.x].isNull())
                            {
                                targets.push_back(pos);
                                break;
                            }
                        }
                    }

                    for (auto target : targets)
                    {
                        auto distance = glm::distance(glm::vec2(movement.position), glm::vec2(target));
                        auto speed = controller.bulletSpeed / distance;
                        auto maxHeight = distance * 0.15F;
                        if (distance < (float)controller.shootMinDistance)
                        {
                            continue;
                        }

                        auto bullet = cmds.spawn(*assets.read(controller.bullet))
                                          .add(Bullet{.origin = movement.position,
                                                      .target = target,
                                                      .baseHeight = position.vec.y + 1.5F,
                                                      .maxHeight = maxHeight,
                                                      .speed = speed})
                                          .entity();

                        cmds.relate(bullet, tileMapEnt, ChildOf{});
                    }
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
