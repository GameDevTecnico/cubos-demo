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
        .withField("maxAcceleration", &Car::maxAcceleration)
        .withField("maxBreaking", &Car::maxBreaking)
        .withField("maxTurn", &Car::maxTurn)
        .withField("forwardDragMul", &Car::forwardDragMul)
        .withField("angularDragMul", &Car::angularDragMul)
        .withField("sideDragMul", &Car::sideDragMul)
        .withField("maxAngularVelocity", &Car::maxAngularVelocity)
        .withField("roadGrip", &Car::roadGrip)
        .withField("turnPeak", &Car::turnPeak)
        .withField("turnDecay", &Car::turnDecay)
        .build();
}
