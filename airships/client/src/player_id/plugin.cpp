#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/input/plugin.hpp>

#include "plugin.hpp"

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(airships::client::PlayerId)
{
    return cubos::core::ecs::TypeBuilder<PlayerId>("airships::client::PlayerId").withField("id", &PlayerId::id).build();
}

void airships::client::playerIdPlugin(Cubos& cubos)
{
    cubos.depends(inputPlugin);

    cubos.component<PlayerId>();

    cubos.system("assign gamepads to Players").call([](Input& input, Query<const PlayerId&> players) {
        int usedGamepads = 0;
        for (auto [player] : players)
        {
            if (usedGamepads < input.gamepadCount())
            {
                input.gamepad(player.id, usedGamepads);
                usedGamepads++;
            }
        }
    });
}