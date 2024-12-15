#include "plugin.hpp"
#include "../random_position/plugin.hpp"
#include "../player_id/plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/vector.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>

#include <queue>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(airships::client::TeamSpawner::Player)
{
    return cubos::core::ecs::TypeBuilder<Player>("airships::client::TeamSpawner::Player")
        .withField("scene", &Player::scene)
        .withField("id", &Player::id)
        .build();
}

CUBOS_REFLECT_IMPL(airships::client::TeamSpawner)
{
    return cubos::core::ecs::TypeBuilder<TeamSpawner>("airships::client::TeamSpawner")
        .withField("boat", &TeamSpawner::boat)
        .withField("players", &TeamSpawner::players)
        .build();
}

void airships::client::teamSpawnerPlugin(Cubos& cubos)
{
    cubos.depends(assetsPlugin);
    cubos.depends(transformPlugin);
    cubos.depends(randomPositionPlugin);
    cubos.depends(playerIdPlugin);

    cubos.component<TeamSpawner>();

    cubos.observer("spawn team")
        .onAdd<TeamSpawner>()
        .call([](Commands cmds, Assets& assets, Query<Entity, TeamSpawner&> query) {
            for (auto [ent, spawner] : query)
            {
                auto boatEnt = cmds.spawn(assets.read(spawner.boat)->blueprint).entity("root");
                cmds.add(boatEnt, RandomPosition{.setYToZero = true});
                cmds.relate(boatEnt, ent, ChildOf{});

                for (auto& player : spawner.players)
                {
                    auto playerEnt = cmds.spawn(assets.read(player.scene)->blueprint).entity("root");
                    cmds.relate(playerEnt, boatEnt, ChildOf{});
                    cmds.add(playerEnt, PlayerId{.id = player.id});
                }
            }
        });
}
