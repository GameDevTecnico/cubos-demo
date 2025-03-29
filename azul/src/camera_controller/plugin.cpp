#include "plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/engine/transform/plugin.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(demo::CameraController)
{
    return cubos::core::ecs::TypeBuilder<CameraController>("demo::CameraController")
        .withField("yaw", &CameraController::yaw)
        .withField("pitch", &CameraController::pitch)
        .withField("distance", &CameraController::distance)
        .withField("center", &CameraController::center)
        .build();
}

void demo::cameraControllerPlugin(Cubos& cubos)
{
    cubos.depends(transformPlugin);

    cubos.component<CameraController>();

    cubos.observer("add position to CameraController")
        .onAdd<CameraController>()
        .without<Position>()
        .call([](Commands cmds, Query<Entity> query) {
            for (auto [entity] : query)
            {
                cmds.add(entity, Position{});
            }
        });

    cubos.system("control Camera").call([](Query<const CameraController&, Rotation&, Position&> query) {
        for (auto [camera, rotation, position] : query)
        {
            auto yawRad = glm::radians(camera.yaw);
            auto pitchRad = glm::radians(camera.pitch);

            glm::vec3 offset =
                glm::vec3(camera.distance * glm::cos(pitchRad) * glm::cos(yawRad), camera.distance * glm::sin(pitchRad),
                          camera.distance * glm::cos(pitchRad) * glm::sin(yawRad));
            position.vec = camera.center + offset;
            rotation.quat = glm::quatLookAt(glm::normalize(-offset), glm::vec3(0.0F, 1.0F, 0.0F));
        }
    });
}
