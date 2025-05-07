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
        float height{15.0F};

        /// @brief Time in seconds until the camera reaches half of the distance to the target.
        float halfTime{1.0F};
        float rotationHalfTime{1.0F};

        bool initialized{false};
    };

    void followPlugin(cubos::engine::Cubos& cubos);
} // namespace demo