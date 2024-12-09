#pragma once

#include <cubos/engine/prelude.hpp>

namespace airships::client
{
    /// @brief Relation which replaces itself by a ChildOf from the source entity to the parent of the target entity.
    struct AutoChildOf
    {
        CUBOS_REFLECT;

        /// @brief How much of the hierarchy to traverse up.
        ///
        /// 0 means simply replacing the relation by a ChildOf with the target entity.
        /// 1 means setting the parent to the parent of the target entity.
        /// 2 means setting the parent to the parent of the parent of the target entity.
        /// And so on.
        int depth{0};
    };

    void autoChildOfPlugin(cubos::engine::Cubos& cubos);
} // namespace airships::client