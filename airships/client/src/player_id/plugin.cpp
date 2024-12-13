#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/input/plugin.hpp>
#include <cubos/engine/assets/plugin.hpp>

#include "plugin.hpp"

using namespace cubos::engine;

static const Asset<InputBindings> Player1BindingsAsset = AnyAsset("7b7463d1-e659-4167-91c3-27267952f071");
static const Asset<InputBindings> PlayerNBindingsAsset = AnyAsset("1581937e-2338-4a38-83c7-309b5977bb40");

CUBOS_REFLECT_IMPL(airships::client::PlayerId)
{
    return cubos::core::ecs::TypeBuilder<PlayerId>("airships::client::PlayerId").withField("id", &PlayerId::id).build();
}

void airships::client::playerIdPlugin(Cubos& cubos)
{
    cubos.depends(inputPlugin);
    cubos.depends(assetsPlugin);

    cubos.component<PlayerId>();

    cubos.observer("load player input bindings")
        .onAdd<PlayerId>()
        .call([](Commands cmds, Input& input, const Assets& assets, Query<const PlayerId&> players) {
            for (auto [player] : players)
            {
                auto asset = player.id == 1 ? Player1BindingsAsset : PlayerNBindingsAsset;
                input.bind(*assets.read(asset), player.id);
            }
        });

    cubos.system("assign gamepads to Players").call([](Input& input, Query<const PlayerId&> players) {
        int usedGamepads = 0;
        for (auto [player] : players)
        {
            if (player.id == 1)
            {
                // Don't give a gamepad to player 1 for now
                continue;
            }

            if (usedGamepads < input.gamepadCount())
            {
                input.gamepad(player.id, usedGamepads);
                usedGamepads++;
            }
        }

        // If there are still gamepads left, give them to player 1
        if (usedGamepads < input.gamepadCount())
        {
            for (auto [player] : players)
            {
                if (player.id == 1)
                {
                    input.gamepad(player.id, usedGamepads);
                    break;
                }
            }
        }
    });
}