#include "plugin.hpp"
#include "../walker/plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>

#include <cubos/engine/input/plugin.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(demo::PlayerController)
{
    return cubos::core::ecs::TypeBuilder<PlayerController>("demo::PlayerController")
        .withField("player", &PlayerController::player)
        .withField("moveX", &PlayerController::moveX)
        .withField("moveY", &PlayerController::moveY)
        .build();
}

void demo::playerControllerPlugin(Cubos& cubos)
{
    cubos.depends(inputPlugin);
    cubos.depends(walkerPlugin);

    cubos.component<PlayerController>();

    cubos.system("do PlayerController")
        .after(inputUpdateTag)
        .call([](Input& input, Query<PlayerController&, Walker&> query) {
            for (auto [controller, walker] : query)
            {
                if (walker.direction != glm::ivec2{0, 0})
                {
                    // The entity is already moving, so we can skip it.
                    continue;
                }

                walker.direction.x =
                    static_cast<int>(glm::round(input.axis(controller.moveX.c_str(), controller.player)));
                if (walker.direction.x == 0)
                {
                    walker.direction.y =
                        static_cast<int>(glm::round(input.axis(controller.moveY.c_str(), controller.player)));
                }
            }
        });
}
