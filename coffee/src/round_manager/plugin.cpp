#include "plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/input/plugin.hpp>
#include <cubos/engine/scene/scene.hpp>
#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/render/target/plugin.hpp>
#include <cubos/engine/render/target/target.hpp>
#include <cubos/engine/render/camera/plugin.hpp>
#include <cubos/engine/render/camera/camera.hpp>
#include <cubos/engine/render/camera/draws_to.hpp>
#include <cubos/engine/collisions/colliding_with.hpp>
#include <cubos/engine/collisions/plugin.hpp>
#include <cubos/engine/physics/solver/plugin.hpp>
#include <cubos/engine/physics/constraints/distance_constraint.hpp>

#include "../car/plugin.hpp"
#include "../player_spawner/plugin.hpp"
#include "../toilet_paper/plugin.hpp"
#include "../score/plugin.hpp"

using namespace cubos::engine;

static const Asset<Scene> MainSceneAsset = AnyAsset("b36dfc06-4ec3-4658-ba67-f8cb3ec787c0");
static const Asset<Scene> RoundManagerSceneAsset = AnyAsset("78d434eb-276b-4c80-9046-0f825bc3ed54");
static const Asset<Scene> EndSceneAsset = AnyAsset("4deb11a6-b535-4994-a68b-1d16f34ba083");

CUBOS_REFLECT_IMPL(coffee::GameRoundSettings)
{
    return cubos::core::ecs::TypeBuilder<GameRoundSettings>("coffee::GameRoundSettings")
        .withField("roundManagerEntity", &GameRoundSettings::roundManagerEntity)
        .withField("currentRound", &GameRoundSettings::currentRound)
        .withField("maxRound", &GameRoundSettings::maxRound)
        .withField("currentTimeBetweenRounds", &GameRoundSettings::currentTimeBetweenRounds)
        .withField("maxTimeBetweenRounds", &GameRoundSettings::maxTimeBetweenRounds)
        .build();
}

CUBOS_REFLECT_IMPL(coffee::RoundManager)
{
    return cubos::core::ecs::TypeBuilder<RoundManager>("coffee::RoundManager").build();
}

CUBOS_REFLECT_IMPL(coffee::Destroy)
{
    return cubos::core::ecs::TypeBuilder<Destroy>("coffee::Destroy").build();
}

CUBOS_REFLECT_IMPL(coffee::Build)
{
    return cubos::core::ecs::TypeBuilder<Build>("coffee::Build").build();
}

CUBOS_REFLECT_IMPL(coffee::BuildEnd)
{
    return cubos::core::ecs::TypeBuilder<BuildEnd>("coffee::BuildEnd").build();
}

CUBOS_REFLECT_IMPL(coffee::RoundPlaying)
{
    return cubos::core::ecs::TypeBuilder<RoundPlaying>("coffee::RoundPlaying").build();
}

CUBOS_REFLECT_IMPL(coffee::WaitingRoundStart)
{
    return cubos::core::ecs::TypeBuilder<WaitingRoundStart>("coffee::WaitingRoundStart")
        .withField("time", &WaitingRoundStart::time)
        .build();
}

CUBOS_REFLECT_IMPL(coffee::ShowEndScreen)
{
    return cubos::core::ecs::TypeBuilder<ShowEndScreen>("coffee::ShowEndScreen")
        .withField("time", &ShowEndScreen::time)
        .withField("maxTime", &ShowEndScreen::maxTime)
        .build();
}

CUBOS_REFLECT_IMPL(coffee::EndArea)
{
    return cubos::core::ecs::TypeBuilder<EndArea>("coffee::EndArea").build();
}

void coffee::roundManagerPlugin(Cubos& cubos)
{
    cubos.depends(transformPlugin);
    cubos.depends(carPlugin);
    cubos.depends(assetsPlugin);
    // cubos.depends(renderTargetPlugin);
    cubos.depends(toiletPaperPlugin);
    cubos.depends(collisionsPlugin);
    cubos.depends(physicsSolverPlugin);
    cubos.depends(scorePlugin);

    cubos.component<RoundManager>();
    cubos.component<Destroy>();
    cubos.component<Build>();
    cubos.component<BuildEnd>();
    cubos.component<RoundPlaying>();
    cubos.component<WaitingRoundStart>();
    cubos.component<ShowEndScreen>();
    cubos.component<EndArea>();
    cubos.resource<GameRoundSettings>();

    cubos.startupSystem("load and spawn the round manager")
        .tagged(assetsTag)
        .call([](Commands cmds, const Assets& assets, GameRoundSettings& roundSettings) {
            roundSettings.roundManagerEntity =
                cmds.spawn(*assets.read(RoundManagerSceneAsset)).named("round-manager").entity();
        });

    cubos.system("detect toilet reached end - reset and add points")
        .call([](Commands cmds, Assets& assets, GameRoundSettings& roundManager, PlayerScores& scores,
                 Query<Entity, ToiletPaper&, CollidingWith&, EndArea&> paperInEnd,
                 Query<Entity, ToiletPaper&, ChildOf&, PlayerOwner&> carWithPaper, Query<RoundPlaying&> roundPlaying) {
            if (roundPlaying.count() == 0)
            {
                return;
            }

            for (auto [entity, toiletPaper, colliding, endArea] : paperInEnd)
            {
                auto match = carWithPaper.pin(0, entity).first();
                if (match)
                {
                    auto [entity, toiletPaper, childOf, playerOwner] = *match;

                    scores.scores[toiletPaper.player - 1] += 100;

                    cmds.remove<RoundPlaying>(roundManager.roundManagerEntity);
                    cmds.add(roundManager.roundManagerEntity, Destroy{});
                }
            }
        });

    cubos.system("destroy scene")
        .call([](Commands cmds, Query<Entity, Destroy&> trigger, Query<Entity> toDestroy,
                 GameRoundSettings& roundSettings) {
            if (trigger.count() == 0)
            {
                return;
            }

            CUBOS_INFO("destroying");
            CUBOS_ASSERT(trigger.count() == 1, "There can only be one round manager");

            for (auto [entity] : toDestroy)
            {
                if (auto match = trigger.at(entity))
                {
                    continue;
                }
                cmds.destroy(entity);
            }
            cmds.remove<Destroy>(roundSettings.roundManagerEntity);

            if (roundSettings.currentRound < roundSettings.maxRound)
            {
                cmds.add(roundSettings.roundManagerEntity, Build{});
                cmds.add(roundSettings.roundManagerEntity, WaitingRoundStart{});
            }
            else
            {
                cmds.add(roundSettings.roundManagerEntity, BuildEnd{});
                cmds.add(roundSettings.roundManagerEntity, ShowEndScreen{});
            }
        });

    cubos.observer("load and spawn the Main scene")
        .onAdd<Build>()
        .call([](Commands cmds, const Assets& assets, GameRoundSettings& roundSettings,
                 Query<Entity, RoundManager&> query) {
            CUBOS_INFO("building");
            cmds.spawn(*assets.read(MainSceneAsset)).named("main");
            for (auto [ent, manager] : query)
            {
                cmds.remove<Build>(ent);
            }
        });

    cubos.observer("load and spawn the end scene")
        .onAdd<BuildEnd>()
        .call([](Commands cmds, const Assets& assets, Query<Entity, RoundManager&> query) {
            CUBOS_INFO("ending game");
            cmds.spawn(*assets.read(EndSceneAsset)).named("end");
            for (auto [ent, manager] : query)
            {
                cmds.remove<BuildEnd>(ent);
            }
        });

    cubos.system("count time between rounds")
        .call([](Commands cmds, GameRoundSettings& roundSettings, DeltaTime& dt,
                 Query<Entity, RoundManager&, WaitingRoundStart&> query) {
            if (roundSettings.currentRound == 0)
            {
                return;
            }

            for (auto [entity, manager, waiting] : query)
            {
                waiting.time += dt.value();
                if (waiting.time > roundSettings.maxTimeBetweenRounds)
                {
                    CUBOS_INFO("starting new round");
                    cmds.remove<WaitingRoundStart>(entity);
                    cmds.add(entity, RoundPlaying{});
                }
            }
        });

    cubos.observer("increase round")
        .onAdd<RoundPlaying>()
        .call(
            [](GameRoundSettings& roundSettings, Query<Entity, RoundManager&> query) { roundSettings.currentRound++; });

    cubos.system("count time between rounds")
        .call([](Commands cmds, GameRoundSettings& roundSettings, DeltaTime& dt, PlayerScores& scores,
                 Query<Entity, RoundManager&, ShowEndScreen&> query) {
            if (roundSettings.currentRound == 0)
            {
                return;
            }

            for (auto [entity, manager, endScreen] : query)
            {
                endScreen.time += dt.value();
                if (endScreen.time > endScreen.maxTime)
                {
                    cmds.remove<ShowEndScreen>(entity);
                    roundSettings.currentRound = 0;
                    for (int i = 0; i < 4; i++)
                    {
                        scores.scores[i] = -1;
                    }
                    cmds.add(entity, Destroy{});
                }
            }
        });
}