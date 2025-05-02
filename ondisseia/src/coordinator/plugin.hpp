#pragma once

#include <string>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/prelude.hpp>

namespace demo
{
    enum class CoordinatorStage
    {
        Intro,
        Level,
        Score,
    };

    struct Coordinator
    {
        CUBOS_REFLECT;

        CoordinatorStage stage{};
        float acc = 0.0F;
    };

    void coordinatorPlugin(cubos::engine::Cubos& cubos);
} // namespace demo
