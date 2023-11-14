#include "controller.hpp"

#include <cubos/core/ecs/component/reflection.hpp>

#include <cubos/core/reflection/external/primitives.hpp>

CUBOS_REFLECT_IMPL(demo::OrbitCameraController)
{
    return cubos::core::ecs::ComponentTypeBuilder<OrbitCameraController>("demo::OrbitCameraController")
        .withField("playerId", &OrbitCameraController::playerId)
        .withField("target", &OrbitCameraController::target)
        .withField("speed", &OrbitCameraController::speed)
        .withField("distance", &OrbitCameraController::distance)
        .withField("theta", &OrbitCameraController::theta)
        .withField("phi", &OrbitCameraController::phi)
        .build();
}
