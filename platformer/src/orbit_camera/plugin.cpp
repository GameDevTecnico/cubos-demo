#include "plugin.hpp"
#include "controller.hpp"
#include "../player/player.hpp"

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

static void inputSystem(Read<Input> input, Query<Write<OrbitCameraController>> cameras, Read<DeltaTime> deltaTime)
{
    for (auto [entity, camera] : cameras)
    {
        camera->theta += input->axis("look-horizontal", camera->playerId) * deltaTime->value * camera->rotationSpeed;
        camera->phi += input->axis("look-vertical", camera->playerId) * deltaTime->value * camera->rotationSpeed;
        camera->phi = glm::clamp(camera->phi, 0.1F, 179.9F);
        camera->distance += input->axis("look-zoom", camera->playerId) * deltaTime->value * camera->zoomSpeed;
        camera->distance = glm::clamp(camera->distance, 1.0F, 100.0F);
    }
}

static void followSystem(Query<Write<OrbitCameraController>, Write<Rotation>> cameras, Query<Write<Position>> positions,
                         Read<DeltaTime> deltaTime, Query<Write<Player>> players)
{
    for (auto [orbitEntity, orbitCamera, orbitRotation] : cameras)
    {
        auto [orbitPosition] = *positions[orbitEntity];
        if (auto targetComponents = positions[orbitCamera->target])
        {
            auto [targetPosition] = *targetComponents;
            orbitCamera->center =
                glm::mix(orbitCamera->center, targetPosition->vec, orbitCamera->speed * deltaTime->value);

            glm::vec3 offset = {glm::cos(glm::radians(orbitCamera->phi)) * glm::cos(glm::radians(orbitCamera->theta)),
                                glm::sin(glm::radians(orbitCamera->phi)),
                                glm::cos(glm::radians(orbitCamera->phi)) * glm::sin(glm::radians(orbitCamera->theta))};

            orbitPosition->vec = orbitCamera->center + offset * orbitCamera->distance;
            orbitRotation->quat = glm::quatLookAt(-offset, {0.0F, 1.0F, 0.0F});

            if (auto playerComponents = players[orbitCamera->target])
            {
                auto [player] = *playerComponents;
                player->forward = glm::normalize(glm::vec3(offset.x, 0.0F, offset.z));
                player->right = glm::normalize(glm::cross(player->forward, {0.0F, 1.0F, 0.0F}));
            }
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
    cubos.system(followSystem);
    cubos.system(registerSystem);
}
