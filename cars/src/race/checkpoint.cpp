#include "checkpoint.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

CUBOS_REFLECT_IMPL(demo::Checkpoint)
{
    return cubos::core::ecs::TypeBuilder<Checkpoint>("demo::Checkpoint")
        .withField("previous", &Checkpoint::previous)
        .withField("isFinishLine", &Checkpoint::isFinishLine)
        .build();
}
