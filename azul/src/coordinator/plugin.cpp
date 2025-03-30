#include "plugin.hpp"

#include "../destroy_tree/plugin.hpp"
#include "../player_controller/plugin.hpp"
#include "../health/plugin.hpp"
#include "../level/plugin.hpp"
#include "../scoreboard/plugin.hpp"

#include <cubos/core/reflection/external/vector.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/ecs/reflection.hpp>

#include <cubos/core/ecs/name.hpp>
#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/scene/scene.hpp>
#include <cubos/engine/input/plugin.hpp>

using namespace cubos::engine;

static const Asset<Scene> MainSceneAsset = AnyAsset("/assets/scenes/main.cubos");
static const Asset<Scene> IntroSceneAsset = AnyAsset("/assets/scenes/intro.cubos");
static const Asset<Scene> LevelSceneAsset = AnyAsset("/assets/scenes/level.cubos");
static const Asset<Scene> ScoreSceneAsset = AnyAsset("/assets/scenes/score.cubos");

CUBOS_REFLECT_IMPL(demo::Coordinator)
{
    return cubos::core::ecs::TypeBuilder<Coordinator>("demo::Coordinator").build();
}

void demo::coordinatorPlugin(Cubos& cubos)
{
    cubos.depends(assetsPlugin);
    cubos.depends(playerControllerPlugin);
    cubos.depends(healthPlugin);
    cubos.depends(levelPlugin);
    cubos.depends(scoreboardPlugin);
    cubos.depends(inputPlugin);

    cubos.resource<Coordinator>(Coordinator{
        .stage = CoordinatorStage::Intro,
    });

    cubos.startupSystem("load and spawn the Main and Intro scenes")
        .tagged(assetsTag)
        .call([](Commands cmds, const Assets& assets) {
            cmds.spawn(*assets.read(MainSceneAsset)).named("main");
            cmds.spawn(*assets.read(IntroSceneAsset)).named("intro").add(Level{});
        });

    cubos.system("restart the scene when only one player remains")
        .call([](Commands cmds, Scoreboard& score, Coordinator& coordinator, const DeltaTime& dt, Assets& assets,
                 Input& input, Query<Entity, const PlayerController&, const Health&> query,
                 Query<Entity, const Level&> queryLevel) {
            bool goToLevel{false};
            bool goToScore{false};

            if (coordinator.stage == CoordinatorStage::Intro)
            {
                coordinator.acc += dt.value();
                if (coordinator.acc > 5.0F)
                {
                    goToLevel = true;
                    coordinator.acc = 0.0F;
                }
            }
            else if (coordinator.stage == CoordinatorStage::Level)
            {
                int alivePlayer = -1;
                for (auto [ent, player, health] : query)
                {
                    if (!score.scores.contains(player.player))
                    {
                        score.scores[player.player] = 0;
                    }

                    if (health.hp > 0)
                    {
                        if (alivePlayer != -1)
                        {
                            return;
                        }

                        alivePlayer = player.player;
                    }
                }

                if (alivePlayer == -1)
                {
                    return;
                }

                coordinator.acc += dt.value();

                if (coordinator.acc > 3.0F)
                {
                    score.increaseScore(alivePlayer);
                    goToScore = true;
                }
            }
            else if (coordinator.stage == CoordinatorStage::Score)
            {
                coordinator.acc += dt.value();
                if (coordinator.acc > 10.0F)
                {
                    goToLevel = true;
                    coordinator.acc = 0.0F;

                    // Reset scores if one is 10
                    bool resetScores{false};
                    for (auto& [player, score] : score.scores)
                    {
                        if (score >= 10)
                        {
                            resetScores = true;
                            break;
                        }
                    }
                    if (resetScores)
                    {
                        for (auto& [player, score] : score.scores)
                        {
                            score = 0;
                        }
                    }
                }
            }

            // Erase past scene
            if (goToLevel || goToScore)
            {
                for (auto [ent, level] : queryLevel)
                {
                    CUBOS_INFO("Found it! {}", ent);
                    CUBOS_ERROR("Entity {} will commit die", ent);
                    cmds.add(ent, DestroyTree{});
                }
            }

            if (goToLevel)
            {
                coordinator.acc = 0.0F;
                coordinator.stage = CoordinatorStage::Level;
                cmds.spawn(*assets.read(LevelSceneAsset)).named("level").add(Level{});
            }
            else if (goToScore)
            {
                coordinator.acc = 0.0F;
                coordinator.stage = CoordinatorStage::Score;
                cmds.spawn(*assets.read(ScoreSceneAsset)).named("score").add(Level{});
            }
        });
}
