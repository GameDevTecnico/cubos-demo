#include "controller.hpp"

#include <cubos/core/ecs/component/reflection.hpp>

#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/glm.hpp>

CUBOS_REFLECT_IMPL(demo::OrbitCameraController)
{
    return cubos::core::ecs::ComponentTypeBuilder<OrbitCameraController>("demo::OrbitCameraController")
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
