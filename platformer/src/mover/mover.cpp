#include "mover.hpp"

#include <cubos/core/ecs/component/reflection.hpp>

#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

CUBOS_REFLECT_IMPL(demo::Mover)
{
    return cubos::core::ecs::ComponentTypeBuilder<Mover>("demo::Mover")
        .withField("from", &Mover::from)
        .withField("to", &Mover::to)
        .withField("speed", &Mover::speed)
        .withField("direction", &Mover::direction)
        .withField("hasStarted", &Mover::hasStarted)
        .build();
}
