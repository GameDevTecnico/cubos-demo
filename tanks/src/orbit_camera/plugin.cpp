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

void orbitCameraPlugin(Cubos& cubos)
{
    cubos.addComponent<OrbitCameraController>();

    cubos.system("inputSystem").call([
    ](Read<Input> input, Query<Write<OrbitCameraController>> cameras, Read<DeltaTime> deltaTime))
    {
        for (auto [entity, camera] : cameras)
        {
            auto lookHorizontal = input->axis("look-horizontal", camera->playerId);
            if (glm::abs(lookHorizontal) < 0.1F)
            {
                lookHorizontal = 0.0F;
            }

            auto lookVertical = input->axis("look-vertical", camera->playerId);
            if (glm::abs(lookVertical) < 0.1F)
            {
                lookVertical = 0.0F;
            }

            camera->theta += lookHorizontal * deltaTime->value * camera->rotationSpeed;
            camera->phi += lookVertical * deltaTime->value * camera->rotationSpeed;
            camera->phi = glm::clamp(camera->phi, 0.1F, 89.0F);

            auto lookZoom = input->axis("look-zoom", camera->playerId);
            if (glm::abs(lookZoom) < 0.1F)
            {
                lookZoom = 0.0F;
            }

            camera->distance += lookZoom * deltaTime->value * camera->zoomSpeed;
            camera->distance = glm::clamp(camera->distance, 15.0F, 100.0F);
        }
    }

    cubos.system("followSystem")
        .call([](Query<Write<OrbitCameraController>, Write<Rotation>> cameras, Query<Write<Position>> positions,
                 Read<DeltaTime> deltaTime, Query<Write<Player>> players))
    {
        for (auto [orbitEntity, orbitCamera, orbitRotation] : cameras)
        {
            auto [orbitPosition] = *positions[orbitEntity];
            if (auto targetComponents = positions[orbitCamera->target])
            {
                auto [targetPosition] = *targetComponents;
                orbitCamera->center =
                    glm::mix(orbitCamera->center, targetPosition->vec, orbitCamera->speed * deltaTime->value);

                glm::vec3 offset = {
                    glm::cos(glm::radians(orbitCamera->phi)) * glm::cos(glm::radians(orbitCamera->theta)),
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

    cubos.system("registerSystem").call([
    ](Write<ActiveCameras> activeCameras, Query<Read<OrbitCameraController>> orbitCameras))
    {
        for (auto [orbitEntity, orbitCamera] : orbitCameras)
        {
            activeCameras->entities[orbitCamera->playerId] = orbitEntity;
        }
    }
}
