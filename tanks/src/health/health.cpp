#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

CUBOS_REFLECT_IMPL(Health)
{
    return cubos::core::ecs::TypeBuilder<Health>("Health").withField("points", &Health::points).build();
}