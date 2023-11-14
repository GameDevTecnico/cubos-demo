#include "dead.hpp"

#include <cubos/core/ecs/component/reflection.hpp>

CUBOS_REFLECT_IMPL(demo::Dead)
{
    return cubos::core::ecs::ComponentTypeBuilder<Dead>("demo::Dead").build();
}
