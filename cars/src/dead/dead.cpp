#include "dead.hpp"

#include <cubos/core/ecs/reflection.hpp>

CUBOS_REFLECT_IMPL(demo::Dead)
{
    return cubos::core::ecs::TypeBuilder<Dead>("demo::Dead").build();
}
