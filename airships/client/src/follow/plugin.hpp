#pragma once

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/prelude.hpp>

namespace airships::client
{
    /// @brief Relation which makes an entity smoothly orbit another entity.
    struct Follow
    {
        CUBOS_REFLECT;

        float distance{1.0F};
        float phi{0.0F};
        float theta{0.0F};

        /// @brief Time in seconds until the camera reaches half of the distance to the target.
        float halfTime{1.0F};

        /// @brief Whether the target entity's rotation is also considered.
        bool considerRotation{false};

        bool initialized{false};
    };

    void followPlugin(cubos::engine::Cubos& cubos);
} // namespace airships::client
