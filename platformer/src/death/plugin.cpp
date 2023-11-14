#include "plugin.hpp"

#include <cubos/engine/collisions/collider.hpp>

#include "../player/player.hpp"
#include "dead.hpp"

using cubos::core::ecs::Commands;
using cubos::core::ecs::Query;
using cubos::core::ecs::Read;
using cubos::core::ecs::Write;

using namespace cubos::engine;

using namespace demo;

#define DEATH_HEIGHT -10.0F

static void fallDetectionSystem(Commands cmds, Query<Write<Player>, Read<Collider>> query)
{
    for (auto [entity, player, collider] : query)
    {
        if (collider->worldAABB.diag[1].y < DEATH_HEIGHT)
        {
            cmds.add(entity, Dead{});
        }
    }
}

static void killSystem(Commands cmds, Query<Read<Player>, Read<Dead>> query)
{
    for (auto [entity, player, dead] : query)
    {
        cmds.destroy(entity);
    }
}

void demo::deathPlugin(Cubos& cubos)
{
    cubos.addComponent<Dead>();

    cubos.system(fallDetectionSystem).before("kill");
    cubos.system(killSystem).tagged("kill");
}
