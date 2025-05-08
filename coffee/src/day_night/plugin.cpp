#include "plugin.hpp"
#include "../progression/plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/engine/render/lights/environment.hpp>
#include <cubos/engine/render/lights/directional.hpp>
#include <cubos/engine/render/lights/spot.hpp>
#include <cubos/engine/render/lights/plugin.hpp>

#include <glm/glm.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(demo::DayNightManager)
{
    return cubos::core::ecs::TypeBuilder<DayNightManager>("demo::DayNightManager")
        .withField("state", &DayNightManager::state)
        .withField("speed", &DayNightManager::speed)
        .withField("nightEnvironment", &DayNightManager::nightEnvironment)
        .withField("nightSunLight", &DayNightManager::nightSunLight)
        .withField("nightCarLightIntensity", &DayNightManager::nightCarLightIntensity)
        .withField("dayEnvironment", &DayNightManager::dayEnvironment)
        .withField("daySunLight", &DayNightManager::daySunLight)
        .withField("dayCarLightIntensity", &DayNightManager::dayCarLightIntensity)
        .build();
}

CUBOS_REFLECT_IMPL(demo::CarLight)
{
    return cubos::core::ecs::TypeBuilder<CarLight>("demo::CarLight").build();
}

void demo::dayNightPlugin(Cubos& cubos)
{
    cubos.depends(lightsPlugin);
    cubos.depends(progressionPlugin);

    cubos.component<DayNightManager>();
    cubos.component<CarLight>();

    cubos.system("update DayNightManager")
        .call([](const Progression& progression, const DeltaTime& dt, Query<DayNightManager&> managers,
                 RenderEnvironment& renderEnvironment, Query<DirectionalLight&> sun,
                 Query<SpotLight&, CarLight&> carLights) {
            CUBOS_ASSERT(managers.count() <= 1, "Only one DayNightManager at a time is allowed.");
            auto match = managers.first();
            if (!match)
            {
                return;
            }
            auto& [manager] = *match;

            // Gradually change the state of the day-night cycle.
            float alpha = 1.0F - glm::pow(0.5F, dt.value() * manager.speed);
            float desiredState;

            if (progression.timeOfDay < progression.dayDuration * 0.5F)
            {
                desiredState = 0.0F;
            }
            else
            {
                desiredState = 1.0F;
            }

            manager.state = glm::mix(manager.state, desiredState, alpha);

            // Update the sun's intensity.
            for (auto [light] : sun)
            {
                light.color = glm::mix(manager.daySunLight.color, manager.nightSunLight.color, manager.state);
                light.intensity =
                    glm::mix(manager.daySunLight.intensity, manager.nightSunLight.intensity, manager.state);
            }

            // Update car lights intensity.
            for (auto [light, carLight] : carLights)
            {
                light.intensity = glm::mix(manager.dayCarLightIntensity, manager.nightCarLightIntensity, manager.state);
            }

            // Update the render environment.
            renderEnvironment.ambient =
                glm::mix(manager.dayEnvironment.ambient, manager.nightEnvironment.ambient, manager.state);
            renderEnvironment.skyGradient[0] =
                glm::mix(manager.dayEnvironment.skyGradient[0], manager.nightEnvironment.skyGradient[0], manager.state);
            renderEnvironment.skyGradient[1] =
                glm::mix(manager.dayEnvironment.skyGradient[1], manager.nightEnvironment.skyGradient[1], manager.state);
        });
}
