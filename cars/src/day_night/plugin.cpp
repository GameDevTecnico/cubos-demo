#include "plugin.hpp"

#include <cubos/engine/render/lights/directional.hpp>
#include <cubos/engine/render/lights/environment.hpp>
#include <cubos/engine/render/lights/spot.hpp>
#include <cubos/engine/render/lights/plugin.hpp>
#include <cubos/engine/input/plugin.hpp>

using namespace cubos::engine;

namespace
{
    struct State
    {
        CUBOS_ANONYMOUS_REFLECT(State);

        bool justPressed = false;
        bool isNight = false;
    };
} // namespace

void demo::dayNightPlugin(Cubos& cubos)
{
    cubos.depends(lightsPlugin);
    cubos.depends(inputPlugin);

    cubos.resource<State>();

    cubos.system("toggle day/night").call([](State& state, Input& input) {
        if (input.pressed("day-night-toggle"))
        {
            if (!state.justPressed)
            {
                state.justPressed = true;
                state.isNight = !state.isNight;
            }
        }
        else
        {
            state.justPressed = false;
        }
    });

    cubos.system("set Render Environment")
        .call([](State& state, RenderEnvironment& environment, Query<DirectionalLight&> sun,
                 Query<SpotLight&> spotLights) {
            for (auto [light] : sun)
            {
                light.intensity = state.isNight ? 0.0f : 0.2f;
            }

            for (auto [light] : spotLights)
            {
                light.intensity = state.isNight ? 0.6f : 0.0f;
            }

            if (state.isNight)
            {
                environment.ambient = {0.1F, 0.1F, 0.1F};
                environment.skyGradient[0] = {0.8F, 0.8F, 1.0F};
                environment.skyGradient[1] = {0.3F, 0.0F, 0.8F};
            }
            else
            {
                environment.ambient = {0.4F, 0.4F, 0.4F};
                environment.skyGradient[0] = {0.6F, 1.0F, 0.8F};
                environment.skyGradient[1] = {0.25F, 0.65F, 1.0F};
            }
        });
}
