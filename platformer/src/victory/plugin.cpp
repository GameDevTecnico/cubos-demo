#include "plugin.hpp"

#include <cubos/engine/collisions/collision_event.hpp>
#include <cubos/engine/assets/assets.hpp>

#include "../player/player.hpp"
#include "victory.hpp"

using cubos::core::ecs::Commands;
using cubos::core::ecs::EventReader;
using cubos::core::ecs::Query;
using cubos::core::ecs::Read;
using cubos::core::ecs::Write;

using namespace cubos::engine;

using namespace demo;

static const Asset<Scene> Scenes[] = {AnyAsset{"541d2b6e-6171-4dd3-96a2-bd53d55b1eb1"}};

static void victorySystem(Commands cmds, Query<Read<Victory>> victories, Query<Read<Player>> players,
                          EventReader<CollisionEvent> collisions, Query<> all, Write<Assets> assets)
{
    for (auto collision : collisions)
    {
        if ((!victories[collision.entity] || !players[collision.other]) &&
            (!victories[collision.other] || !players[collision.entity]))
        {
            continue;
        }

        auto [victory] = *victories[collision.entity];
        auto [player] = *players[collision.other];

        for (auto [entity] : all)
        {
            cmds.destroy(entity);
        }
        cmds.spawn(assets->read(Scenes[victory->nextScene])->blueprint);
        break;
    }
}

void demo::victoryPlugin(Cubos& cubos)
{
    cubos.addComponent<Victory>();
    cubos.system(victorySystem).after("cubos.collisions");
}
