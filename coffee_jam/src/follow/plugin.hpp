#pragma once

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/prelude.hpp>

namespace demo
{
    /// @brief Relation which makes an entity smoothly follow another entity at a given distance and angle.
    struct Follow
    {
        CUBOS_REFLECT;

        float distance{1.0F};
        float phi{0.0F};
        float theta{0.0F};
        float speed{1.0F};
    };

    void followPlugin(cubos::engine::Cubos& cubos);
} // namespace demo
