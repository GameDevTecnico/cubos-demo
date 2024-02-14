#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

CUBOS_REFLECT_IMPL(Death)
{
    return cubos::core::ecs::TypeBuilder<Death>("Death").build();
}
