#pragma once

#include <glm/vec2.hpp>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/prelude.hpp>

namespace demo
{
    /// @brief Component used to identify static entities which are present in the tile-map.
    struct Object
    {
        CUBOS_REFLECT;

        /// @brief Position in the tile-map.
        glm::ivec2 position{0, 0};

        /// @brief How many tiles the object occupies.
        glm::ivec2 size{1, 1};

        bool initialized = false;
    };

    void objectPlugin(cubos::engine::Cubos& cubos);
} // namespace demo
