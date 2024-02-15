#include "car.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/glm.hpp>

CUBOS_REFLECT_IMPL(demo::Car)
{
    return cubos::core::ecs::TypeBuilder<Car>("demo::Car")
        .withField("player", &Car::player)
        .withField("linearVelocity", &Car::linearVelocity)
        .withField("angularVelocity", &Car::angularVelocity)
        .withField("wheelAngle", &Car::wheelAngle)
        .withField("wheelBase", &Car::wheelBase)
        .withField("acceleration", &Car::acceleration)
        .withField("braking", &Car::braking)
        .withField("maxVelocity", &Car::maxVelocity)
        .withField("steeringSpeed", &Car::steeringSpeed)
        .withField("maxWheelAngle", &Car::maxWheelAngle)
        .withField("wheelGrip", &Car::wheelGrip)
        .withField("forwardDrag", &Car::forwardDrag)
        .withField("forwardDragCoefficient", &Car::forwardDragCoefficient)
        .withField("lateralDrag", &Car::lateralDrag)
        .withField("lateralDragCoefficient", &Car::lateralDragCoefficient)
        .build();
}
