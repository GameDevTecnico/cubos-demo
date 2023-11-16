#include "cannon.hpp"

#include <cubos/core/ecs/component/reflection.hpp>

#include <cubos/core/reflection/external/primitives.hpp>

CUBOS_REFLECT_IMPL(demo::Cannon)
{
    return cubos::core::ecs::ComponentTypeBuilder<Cannon>("demo::Cannon")
        .withField("bullet", &Cannon::bullet)
        .withField("bulletTime", &Cannon::bulletTime)
        .withField("timer", &Cannon::timer)
        .build();
}
