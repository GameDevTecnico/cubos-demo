#include "obstacle.hpp"

#include <cubos/core/ecs/reflection.hpp>

CUBOS_REFLECT_IMPL(demo::Obstacle)
{
    return cubos::core::ecs::TypeBuilder<Obstacle>("demo::Obstacle").build();
}
