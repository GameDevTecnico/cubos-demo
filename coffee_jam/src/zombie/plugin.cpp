#include "plugin.hpp"
#include "../walker/plugin.hpp"
#include "../player_controller/plugin.hpp"
#include "../tile_map/plugin.hpp"
#include "cubos/engine/assets/plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/log.hpp>

#include <cubos/engine/input/plugin.hpp>
#include <cubos/engine/scene/scene.hpp>
#include <cubos/engine/assets/asset.hpp>
#include <cubos/engine/assets/assets.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/fixed_step/plugin.hpp>
#include <glm/ext/vector_int2.hpp>

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
    cubos.depends(tileMapPlugin);
    cubos.depends(assetsPlugin);
    cubos.depends(transformPlugin);
    static Asset<Scene> ZombieAsset = AnyAsset("4d8ff909-46df-49c4-adf0-ca409b34510f");
    std::srand(std::time(nullptr));

    cubos.component<ZombieController>();

    /*cubos.startupSystem("Initialize zombie")
        .tagged(assetsTag)
        .call([](Commands cmds, const Assets& assets, Query<Entity, TileMap&> query) {
            auto zombieEnt = cmds.spawn(assets.read(ZombieAsset)->blueprint).entity("base.body");
            cmds.add(zombieEnt, Walker{.position = {32, 32}, .initialized = true});
            for (auto [entity, map] : query)
            {
                cmds.relate(zombieEnt, entity, ChildOf{});
                break;
            }
        });*/

    cubos.system("do ZombieController")
        .call([](Commands cmds, Query<Entity, ZombieController&, Walker&, ChildOf&, TileMap&, Entity> zombies,
                 Query<Entity, PlayerController&, Walker&> players) {
            std::vector<glm::ivec2> playerPositions;
            for (auto [playerEnt, controller, walker] : players)
            {
                playerPositions.push_back(walker.position);
            }
            for (auto [zombieEnt, controller, walker, childof, map, mapEnt] : zombies)
            {
                if (walker.direction != glm::ivec2{0, 0})
                {
                    // The entity is already moving, so we can skip it.
                    continue;
                }
                auto target = walker.position + walker.facing;
                if (target.x < 0 || target.x >= map.entities.size() || target.y < 0 || target.y >= map.entities.size())
                {
                    // The position the player is facing is out of bounds.
                    continue;
                }

                // Choose closest target
                glm::ivec2 positionTargeted = playerPositions[0];
                for (auto position : playerPositions)
                {
                    if (glm::distance((glm::vec2)walker.position, (glm::vec2)position) <
                        glm::distance((glm::vec2)walker.position, (glm::vec2)positionTargeted))
                    {
                        positionTargeted = position;
                    }
                }

                // Move the character
                (positionTargeted.x > walker.position.x) ? walker.direction.x = 1 : walker.direction.x = -1;
                int randomChoice = std::rand() % 2;
                if (randomChoice)
                {
                    walker.direction.x = 0;
                }
                if (walker.direction.x == 0)
                {
                    (positionTargeted.y > walker.position.y) ? walker.direction.y = 1 : walker.direction.y = -1;
                }
            }
        });
}
