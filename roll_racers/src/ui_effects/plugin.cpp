#include "plugin.hpp"
#include "../score/plugin.hpp"
#include "../car/plugin.hpp"
#include "../round_manager/plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/engine/ui/text/plugin.hpp>
#include <cubos/engine/ui/text/text.hpp>
#include <cubos/engine/ui/canvas/element.hpp>
#include <cubos/engine/ui/canvas/plugin.hpp>
#include <cubos/engine/scene/scene.hpp>
#include <cubos/engine/render/camera/draws_to.hpp>
#include <cubos/engine/render/camera/camera.hpp>
#include <cubos/engine/render/camera/plugin.hpp>
#include <cubos/engine/render/split_screen/plugin.hpp>
#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/input/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>

using namespace cubos::engine;

namespace
{
    struct State
    {
        CUBOS_ANONYMOUS_REFLECT(State);

        /// @brief Current state of the cycle.
        float state = 0.0F;
    };
} // namespace

CUBOS_REFLECT_IMPL(roll_racers::UIBlink)
{
    return cubos::core::ecs::TypeBuilder<UIBlink>("roll_racers::UIBlink").build();
}

CUBOS_REFLECT_IMPL(roll_racers::ScoreUIManager)
{
    return cubos::core::ecs::TypeBuilder<ScoreUIManager>("roll_racers::ScoreUIManager").build();
}

CUBOS_REFLECT_IMPL(roll_racers::ScoreUI)
{
    return cubos::core::ecs::TypeBuilder<ScoreUI>("roll_racers::ScoreUI").withField("player", &ScoreUI::player).build();
}

CUBOS_REFLECT_IMPL(roll_racers::ScoreboardPlayerName)
{
    return cubos::core::ecs::TypeBuilder<ScoreboardPlayerName>("roll_racers::ScoreboardPlayerName")
        .withField("order", &ScoreboardPlayerName::order)
        .build();
}

CUBOS_REFLECT_IMPL(roll_racers::ScoreboardPlayerScore)
{
    return cubos::core::ecs::TypeBuilder<ScoreboardPlayerScore>("roll_racers::ScoreboardPlayerScore")
        .withField("order", &ScoreboardPlayerScore::order)
        .build();
}

CUBOS_REFLECT_IMPL(roll_racers::InstructionsUI)
{
    return cubos::core::ecs::TypeBuilder<InstructionsUI>("roll_racers::InstructionsUI").build();
}

static const Asset<Scene> ScoreUISceneAsset = AnyAsset("d684383c-77b5-47c4-b017-724d42b84ca7");

void roll_racers::uiEffectsPlugin(Cubos& cubos)
{
    cubos.depends(uiTextPlugin);
    cubos.depends(uiCanvasPlugin);
    cubos.depends(cameraPlugin);
    cubos.depends(assetsPlugin);
    cubos.depends(inputPlugin);
    cubos.depends(transformPlugin);
    cubos.depends(splitScreenPlugin);
    cubos.depends(scorePlugin);
    cubos.depends(carPlugin);
    cubos.depends(roundManagerPlugin);

    cubos.component<UIBlink>();
    cubos.component<ScoreUIManager>();
    cubos.component<ScoreUI>();
    cubos.component<ScoreboardPlayerName>();
    cubos.component<ScoreboardPlayerScore>();
    cubos.component<InstructionsUI>();

    cubos.resource<State>();

    cubos.system("apply UI effects").call([](const DeltaTime& dt, State& state, Query<UIText&, UIBlink&> textElements) {
        // Update time
        state.state += 4.0F * dt.value();

        // Update text transparency
        for (auto [text, _] : textElements)
        {
            text.color = glm::vec4(1.0F, 1.0F, 1.0F, 0.5F + 0.5F * glm::sin(state.state));
        }
    });

    cubos.system("show score UI for each player")
        .after(splitScreenTag)
        .call([](Commands cmds, Assets& assets, Query<ScoreUIManager&> targets, Query<Entity, const Camera&> cameras,
                 Query<Entity, const Camera&, const PlayerCameraOwner&, const DrawsTo&, Entity, ScoreUIManager&>
                     viewports,
                 Query<UIElement&> elements) {
            for (auto [manager] : targets)
            {
                // Remove UIs that are no longer needed
                std::vector<Entity> removedCameras;
                for (auto& [cameraEntity, _] : manager.scoreUIs)
                {
                    if (!cameras.at(cameraEntity))
                    {
                        removedCameras.push_back(cameraEntity);
                    }
                }
                for (auto [cameraEntity, camera] : cameras)
                {
                    if (!camera.active && manager.scoreUIs.contains(cameraEntity))
                    {
                        removedCameras.push_back(cameraEntity);
                    }
                }
                for (auto& cameraEntity : removedCameras)
                {
                    cmds.destroy(manager.scoreUIs[cameraEntity]);
                    manager.scoreUIs.erase(cameraEntity);
                }
            }

            for (auto [cameraEntity, camera, cameraOwner, drawsTo, targetEntity, manager] : viewports)
            {
                if (camera.active && !manager.scoreUIs.contains(cameraEntity))
                {
                    // Add new UI
                    manager.scoreUIs[cameraEntity] =
                        cmds.spawn(*assets.read(ScoreUISceneAsset))
                            .add(UIElement{.offset = glm::vec2(160.0F, -40.0F),
                                           .anchor = glm::vec2(drawsTo.viewportOffset.x,
                                                               drawsTo.viewportOffset.y + drawsTo.viewportSize.y)})
                            .add(ScoreUI{.player = cameraOwner.player})
                            .entity();

                    cmds.relate(manager.scoreUIs[cameraEntity], targetEntity, ChildOf{});
                }
                else if (manager.scoreUIs.contains(cameraEntity))
                {
                    // Update UI position
                    auto element = elements.at(manager.scoreUIs[cameraEntity]);
                    std::get<0>(element.value()).anchor =
                        glm::vec2(drawsTo.viewportOffset.x, drawsTo.viewportOffset.y + drawsTo.viewportSize.y);
                }
            }
        });

    cubos.system("update score display")
        .call([](Commands commands, Query<Entity, const UIText&, const ChildOf&, const ScoreUI&> scoreDisplays,
                 PlayerScores& scores) {
            for (auto [entity, _, __, display] : scoreDisplays)
            {
                commands.remove<UIText>(entity);
                commands.add(entity, UIText{.text = std::format("{:05d}", scores.scores[display.player - 1]),
                                            .fontSize = 80.0F,
                                            .fontAtlas = AnyAsset("bd0387d2-af3d-4c65-8561-33f5bcf6ab37")});
            }
        });

    cubos.system("update scoreboard")
        .call([](Commands commands, PlayerScores& scores, Query<Entity, const ScoreboardPlayerName&> boardNames,
                 Query<Entity, const ScoreboardPlayerScore&> boardScores) {
            constexpr glm::vec4 playerColors[4] = {
                glm::vec4(1.0F, 0.25F, 0.25F, 1.0F), glm::vec4(0.25F, 1.0F, 0.25F, 1.0F),
                glm::vec4(1.0F, 0.5F, 1.0F, 1.0F), glm::vec4(0.25F, 0.25F, 1.0F, 1.0F)};

            // Sort
            std::vector<std::tuple<int, int>> sortedScores;
            for (int i = 0; i < 4; i++)
            {
                if (scores.scores[i] == -1)
                {
                    sortedScores.push_back({-1, -1});
                }
                else
                {
                    sortedScores.push_back({i + 1, scores.scores[i]});
                }
            }
            std::sort(sortedScores.begin(), sortedScores.end(),
                      [](std::tuple<int, int> a, std::tuple<int, int> b) { return std::get<1>(a) > std::get<1>(b); });

            // Update
            for (auto [entity, boardName] : boardNames)
            {
                int player = std::get<0>(sortedScores.at(boardName.order - 1));
                if (player != -1)
                {
                    commands.add(entity, UIText{.text = std::format("Player {}", player),
                                                .color = playerColors[player - 1],
                                                .fontSize = 80.0F,
                                                .fontAtlas = AnyAsset("bd0387d2-af3d-4c65-8561-33f5bcf6ab37")});
                }
                commands.remove<ScoreboardPlayerName>(entity);
            }
            for (auto [entity, boardScore] : boardScores)
            {
                int player = std::get<0>(sortedScores.at(boardScore.order - 1));
                int score = std::get<1>(sortedScores.at(boardScore.order - 1));
                if (score != -1)
                {
                    commands.add(entity, UIText{.text = std::format("{:05d}",
                                                                    std::get<1>(sortedScores.at(boardScore.order - 1))),
                                                .color = playerColors[player - 1],
                                                .fontSize = 80.0F,
                                                .fontAtlas = AnyAsset("bd0387d2-af3d-4c65-8561-33f5bcf6ab37")});
                }
                commands.remove<ScoreboardPlayerScore>(entity);
            }
        });

    cubos.system("delete instructions on play")
        .call([](Commands cmds, Input& input, GameRoundSettings& roundManager, Query<Entity, InstructionsUI&> query) {
            if (input.justPressed("play", 1) || input.justPressed("play", 2) || input.justPressed("play", 3) ||
                input.justPressed("play", 4) || roundManager.currentRound != 0)
            {
                for (auto [entity, _] : query)
                {
                    cmds.destroy(entity);
                }
            }
        });
}
