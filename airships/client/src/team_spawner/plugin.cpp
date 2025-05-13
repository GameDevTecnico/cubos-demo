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
        .withField("skin", &Player::skin)
        .withField("id", &Player::id)
        .build();
}

CUBOS_REFLECT_IMPL(airships::client::TeamSpawner)
{
    return cubos::core::ecs::TypeBuilder<TeamSpawner>("airships::client::TeamSpawner")
        .withField("boatScene", &TeamSpawner::boatScene)
        .withField("playerScene", &TeamSpawner::playerScene)
        .withField("players", &TeamSpawner::players)
        .build();
}

namespace
{
    struct TeamSpawnerDestroyDetect
    {
        CUBOS_REFLECT;
    };

    CUBOS_REFLECT_IMPL(TeamSpawnerDestroyDetect)
    {
        return cubos::core::ecs::TypeBuilder<TeamSpawnerDestroyDetect>("airships::client::TeamSpawnerDestroyDetect")
            .build();
    }
} // namespace

void airships::client::teamSpawnerPlugin(Cubos& cubos)
{
    cubos.depends(assetsPlugin);
    cubos.depends(transformPlugin);
    cubos.depends(randomPositionPlugin);
    cubos.depends(playerIdPlugin);
    cubos.depends(boatSkinPlugin);

    cubos.component<TeamSpawner>();
    cubos.component<TeamSpawnerDestroyDetect>();

    cubos.observer("spawn team")
        .onAdd<TeamSpawner>()
        .call([](Commands cmds, Assets& assets, Query<Entity, TeamSpawner&> query) {
            for (auto [ent, spawner] : query)
            {
                auto boatEnt = cmds.spawn(assets.read(spawner.boatScene)->blueprint()).named("boat").entity();
                cmds.add(boatEnt, spawner.boatSkin);
                cmds.add(boatEnt, RandomPosition{.minHeight = 0, .maxHeight = 0});
                cmds.add(boatEnt, TeamSpawnerDestroyDetect{});
                cmds.relate(boatEnt, ent, ChildOf{});

                for (auto& player : spawner.players)
                {
                    auto playerEnt = cmds.spawn(assets.read(spawner.playerScene)->blueprint())
                                         .named("player" + std::to_string(player.id))
                                         .entity();
                    cmds.relate(playerEnt, boatEnt, ChildOf{});
                    cmds.add(playerEnt, player.skin);
                    cmds.add(playerEnt, PlayerId{.id = player.id});
                }
            }
        });

    cubos.observer("detect team destruction")
        .onRemove<TeamSpawnerDestroyDetect>()
        .call([](Commands cmds, Query<const ChildOf&, Entity> query) {
            for (auto [childOf, team] : query)
            {
                cmds.destroy(team);
            }
        });
}
