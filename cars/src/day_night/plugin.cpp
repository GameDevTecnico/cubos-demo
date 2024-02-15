#include "plugin.hpp"

#include <cubos/engine/renderer/directional_light.hpp>
#include <cubos/engine/renderer/environment.hpp>
#include <cubos/engine/renderer/spot_light.hpp>
#include <cubos/engine/input/input.hpp>

using namespace cubos::engine;

namespace
{
    struct State
    {
        bool justPressed = false;
        bool isNight = false;
    };
} // namespace

void demo::dayNightPlugin(Cubos& cubos)
{
    cubos.addResource<State>();

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

    cubos.system("set Renderer Environment")
        .tagged("cubos.renderer.frame")
        .call([](State& state, RendererEnvironment& environment, Query<DirectionalLight&> sun,
                 Query<SpotLight&> spotLights) {
            for (auto [light] : sun)
            {
                light.intensity = state.isNight ? 0.0f : 1.0f;
            }

            for (auto [light] : spotLights)
            {
                light.intensity = state.isNight ? 1.0f : 0.0f;
            }

            if (state.isNight)
            {
                environment.ambient = {0.2F, 0.2F, 0.2F};
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
