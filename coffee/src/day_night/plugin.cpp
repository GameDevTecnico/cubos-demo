#include "plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/engine/render/lights/environment.hpp>
#include <cubos/engine/render/lights/directional.hpp>
#include <cubos/engine/render/lights/spot.hpp>
#include <cubos/engine/render/lights/plugin.hpp>

#include <glm/glm.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(coffee::DayNightManager)
{
    return cubos::core::ecs::TypeBuilder<DayNightManager>("coffee::DayNightManager")
        .withField("state", &DayNightManager::state)
        .withField("desiredState", &DayNightManager::desiredState)
        .withField("speed", &DayNightManager::speed)
        .withField("nightEnvironment", &DayNightManager::nightEnvironment)
        .withField("nightSunLight", &DayNightManager::nightSunLight)
        .withField("dayEnvironment", &DayNightManager::dayEnvironment)
        .withField("daySunLight", &DayNightManager::daySunLight)
        .build();
}

CUBOS_REFLECT_IMPL(coffee::CarLight)
{
    return cubos::core::ecs::TypeBuilder<CarLight>("coffee::CarLight").build();
}

void coffee::dayNightPlugin(Cubos& cubos)
{
    cubos.depends(lightsPlugin);

    cubos.component<DayNightManager>();
    cubos.component<CarLight>();

    cubos.system("update DayNightManager")
        .call([](const DeltaTime& dt, Query<DayNightManager&> managers, RenderEnvironment& renderEnvironment,
                 Query<DirectionalLight&> sun, Query<SpotLight&, CarLight&> carLights) {
            CUBOS_ASSERT(managers.count() <= 1, "Only one DayNightManager at a time is allowed.");
            auto match = managers.first();
            if (!match)
            {
                return;
            }
            auto& [manager] = *match;

            // Gradually change the state of the day-night cycle.
            float alpha = 1.0F - glm::pow(0.5F, dt.value() * manager.speed);
            manager.state = glm::mix(manager.state, manager.desiredState, alpha);

            // Update the sun's intensity.
            for (auto [light] : sun)
            {
                light.color = glm::mix(manager.daySunLight.color, manager.nightSunLight.color, manager.state);
                light.intensity =
                    glm::mix(manager.daySunLight.intensity, manager.nightSunLight.intensity, manager.state);
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
