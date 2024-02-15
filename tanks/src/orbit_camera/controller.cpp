#include "controller.hpp"

#include <cubos/core/ecs/reflection.hpp>

#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/glm.hpp>

CUBOS_REFLECT_IMPL(OrbitCameraController)
{
    return cubos::core::ecs::TypeBuilder<OrbitCameraController>("OrbitCameraController")
        .withField("playerId", &OrbitCameraController::playerId)
        .withField("target", &OrbitCameraController::target)
        .withField("center", &OrbitCameraController::center)
        .withField("speed", &OrbitCameraController::speed)
        .withField("rotationSpeed", &OrbitCameraController::rotationSpeed)
        .withField("zoomSpeed", &OrbitCameraController::zoomSpeed)
        .withField("distance", &OrbitCameraController::distance)
        .withField("theta", &OrbitCameraController::theta)
        .withField("phi", &OrbitCameraController::phi)
        .build();
}
