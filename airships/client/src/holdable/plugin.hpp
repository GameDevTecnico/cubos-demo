#pragma once

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/prelude.hpp>

namespace airships::client
{
    /// @brief Component used to identify entities which can be held by other entities.
    struct Holdable
    {
        CUBOS_REFLECT;

        /// @brief Type of item.
        std::string type = "";
    };

    void holdablePlugin(cubos::engine::Cubos& cubos);
} // namespace airships::client
