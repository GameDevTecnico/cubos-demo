#pragma once

#include <cubos/core/reflection/reflect.hpp>
#include <cubos/engine/prelude.hpp>

namespace demo
{
    struct Checkpoint
    {
        CUBOS_REFLECT;

        // Handle of the previous checkpoint. Racers must pass through the previous checkpoint before
        // being able to pass through this one.
        cubos::engine::Entity previous;

        // Whether this checkpoint is the finish line.
        bool isFinishLine = false;
    };
} // namespace demo
