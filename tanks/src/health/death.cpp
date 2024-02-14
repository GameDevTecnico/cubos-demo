#include "death.hpp"

#include <cubos/core/ecs/reflection.hpp>

CUBOS_REFLECT_IMPL(Death)
{
    return cubos::core::ecs::TypeBuilder<Death>("Death").build();
}
