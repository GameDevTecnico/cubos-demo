#include "plugin.hpp"
#include "../progression/plugin.hpp"

#include <cubos/engine/render/lights/environment.hpp>
#include <cubos/engine/render/lights/directional.hpp>
#include <cubos/engine/render/lights/spot.hpp>
#include <cubos/engine/render/lights/plugin.hpp>

#include <glm/glm.hpp>

using namespace cubos::engine;

namespace
{
    struct State
    {
        CUBOS_ANONYMOUS_REFLECT(State);

        /// @brief Current state of the day-night cycle, where 0.0 is full day and 1.0 is full night.
        float state = 0.0F;

        /// @brief How fast the state changes.
        float speed = 0.75F;
    };
} // namespace

void demo::dayNightPlugin(Cubos& cubos)
{
    cubos.depends(lightsPlugin);
    cubos.depends(progressionPlugin);

    cubos.resource<State>();

    cubos.system("update RenderEnvironment")
        .call([](const Progression& progression, const DeltaTime& dt, State& state,
                 RenderEnvironment& renderEnvironment, Query<DirectionalLight&> sun, Query<SpotLight&> lanterns) {
            // Gradually change the state of the day-night cycle.
            float alpha = 1.0F - glm::pow(0.5F, dt.value() * state.speed);
            float desiredState;

            if (progression.timeOfDay < progression.dayDuration * 0.5F)
            {
                desiredState = 0.0F;
            }
            else
            {
                desiredState = 1.0F;
            }

            state.state = glm::mix(state.state, desiredState, alpha);

            // Update the sun's intensity.
            for (auto [light] : sun)
            {
                light.intensity = glm::mix(0.75F, 0.0F, state.state);
            }

            // Update spot lights intensity.
            for (auto [light] : lanterns)
            {
                light.intensity = glm::mix(0.0F, 1.5F, state.state);
            }

            // Update the render environment.
            renderEnvironment.ambient = glm::mix(glm::vec3(0.4F), glm::vec3(0.1F), state.state);
            renderEnvironment.skyGradient[0] = glm::mix({0.6F, 1.0F, 0.8F}, glm::vec3(0.1F), state.state);
            renderEnvironment.skyGradient[1] = glm::mix({0.6F, 1.0F, 0.8F}, glm::vec3(0.1F), state.state);
        });
}
