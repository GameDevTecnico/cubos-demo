#include "plugin.hpp"
#include "../scoreboard/plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/external/unordered_map.hpp>

#include <cubos/engine/prelude.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/scene/plugin.hpp>
#include <cubos/engine/assets/plugin.hpp>

#include <random>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(demo::ScoreIndicator)
{
    return cubos::core::ecs::TypeBuilder<ScoreIndicator>("demo::ScoreIndicator")
        .withField("numeral", &ScoreIndicator::numeral)
        .withField("rotatingAnimation", &ScoreIndicator::rotatingAnimation)
        .withField("playerScenes", &ScoreIndicator::playerScenes)
        .build();
}

void demo::scoreIndicatorPlugin(Cubos& cubos)
{
    cubos.depends(transformPlugin);
    cubos.depends(scenePlugin);
    cubos.depends(assetsPlugin);
    cubos.depends(scoreboardPlugin);
    cubos.depends(numeralsPlugin);

    cubos.component<ScoreIndicator>();

    cubos.observer("create ScoreIndicator")
        .onAdd<ScoreIndicator>()
        .call([](Commands cmds, Assets& assets, const Scoreboard& scoreboard,
                 Query<Entity, const ScoreIndicator&> query) {
            for (auto [ent, indicator] : query)
            {
                int bestPlayer = -1;
                for (auto [player, score] : scoreboard.scores)
                {
                    if (bestPlayer == -1)
                    {
                        bestPlayer = player;
                    }
                    else if (score > scoreboard.scores.at(bestPlayer))
                    {
                        bestPlayer = player;
                    }
                }

                float playerI = 0.0F;
                float playerDistance = 5.0F;
                float playerOffset = 10.5F - playerDistance * ((float)scoreboard.scores.size() - 1.0F) / 2.0F;

                for (auto [player, score] : scoreboard.scores)
                {
                    if (player == bestPlayer)
                    {
                        continue;
                    }

                    if (!indicator.playerScenes.contains(player))
                    {
                        CUBOS_ERROR("No score indicator scene for player {}", player);
                        continue;
                    }

                    float x = playerOffset + playerI * playerDistance;

                    auto animation = indicator.rotatingAnimation;
                    animation.time = (float)(rand() % 1000) / 1000.0F;

                    auto playerRootEnt =
                        cmds.create().add(Position{{x, -1.5F, 8.0F}}).add(animation).relatedTo(ent, ChildOf{}).entity();
                    auto playerEnt = cmds.spawn(*assets.read(indicator.playerScenes.at(player))).entity();
                    cmds.relate(playerEnt, playerRootEnt, ChildOf{});

                    auto numeral = indicator.numeral;
                    numeral.number = score;
                    cmds.create().add(numeral).add(Position{{x, -2.75F, 8.0F}}).relatedTo(ent, ChildOf{});
                    playerI += 1.0F;
                }

                if (bestPlayer != -1)
                {
                    auto animation = indicator.rotatingAnimation;
                    animation.time = (float)(rand() % 1000) / 1000.0F;

                    auto playerRootEnt = cmds.create()
                                             .add(Position{{8.0F, 3.5F, 8.0F}})
                                             .add(Scale{1.5F})
                                             .add(animation)
                                             .relatedTo(ent, ChildOf{})
                                             .entity();
                    auto playerEnt = cmds.spawn(*assets.read(indicator.playerScenes.at(bestPlayer))).entity();
                    cmds.relate(playerEnt, playerRootEnt, ChildOf{});

                    auto numeral = indicator.numeral;
                    numeral.number = scoreboard.scores.at(bestPlayer);
                    cmds.create()
                        .add(numeral)
                        .add(Position{{8.0F, 2.0F, 8.0F}})
                        .add(Scale{1.5F})
                        .relatedTo(ent, ChildOf{});
                }

                cmds.remove<ScoreIndicator>(ent);
            }
        });
}
