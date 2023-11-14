#include "plugin.hpp"
#include "controller.hpp"

#include <cubos/engine/input/input.hpp>
#include <cubos/engine/transform/position.hpp>
#include <cubos/engine/transform/rotation.hpp>
#include <cubos/engine/renderer/plugin.hpp>

#include <glm/glm.hpp>

using cubos::core::ecs::Query;
using cubos::core::ecs::Read;
using cubos::core::ecs::Write;

using namespace cubos::engine;

using namespace demo;

static void inputSystem(Read<Input> input, Query<Read<OrbitCameraController>, Write<Rotation>> cameras,
                        Query<Write<Position>> positions, Read<DeltaTime> deltaTime)
{
    for (auto [orbitEntity, orbitCamera, orbitRotation] : cameras)
    {
        auto [orbitPosition] = *positions[orbitEntity];
        if (auto targetComponents = positions[orbitCamera->target])
        {
            auto [targetPosition] = *targetComponents;

            glm::vec3 offset = {glm::cos(glm::radians(orbitCamera->phi)) * glm::cos(glm::radians(orbitCamera->theta)),
                                glm::sin(glm::radians(orbitCamera->phi)),
                                glm::cos(glm::radians(orbitCamera->phi)) * glm::sin(glm::radians(orbitCamera->theta))};
            auto orbitDesiredPosition = targetPosition->vec + offset * orbitCamera->distance;
            orbitPosition->vec =
                glm::mix(orbitPosition->vec, orbitDesiredPosition, orbitCamera->speed * deltaTime->value);
            orbitRotation->quat = glm::quatLookAt(-offset, {0.0F, 1.0F, 0.0F});
        }
    }
}

static void registerSystem(Write<ActiveCameras> activeCameras, Query<Read<OrbitCameraController>> orbitCameras)
{
    for (auto [orbitEntity, orbitCamera] : orbitCameras)
    {
        activeCameras->entities[orbitCamera->playerId] = orbitEntity;
    }
}

void demo::orbitCameraPlugin(Cubos& cubos)
{
    cubos.addComponent<OrbitCameraController>();
    cubos.system(inputSystem);
    cubos.system(registerSystem);
}
