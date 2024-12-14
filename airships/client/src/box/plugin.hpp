#pragma once
#include <vector>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/prelude.hpp>
#include <cubos/engine/transform/position.hpp>

namespace airships::client
{
    /// @brief Component used to identify a box that can store holdable items.
    struct Box
    {
        CUBOS_REFLECT;

        /// @brief Currently free positions where items can be stored.
        /// Last position is relative to InterpolationOf Box, and the rest are relative to each next slot.
        std::vector<cubos::engine::Position> freeSlots{};

        /// @brief Type of item to be stored in this box.
        std::string type = "";
    };

    void boxPlugin(cubos::engine::Cubos& cubos);
} // namespace airships::client
