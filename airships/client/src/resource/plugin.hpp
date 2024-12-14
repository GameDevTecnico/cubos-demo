#pragma once

#include <cubos/core/reflection/reflect.hpp>
#include <cubos/engine/prelude.hpp>
#include <cubos/engine/api.hpp>

namespace airships::client
{
    struct ResourceInfo
    {
        CUBOS_REFLECT;

        enum class ResourceTypes
        {
            Cannonball,
            Coal,
            Wood
        };

        ResourceTypes type;
    };

    /// @brief plugin to handle resources
    /// @param cubos
    void resourcesPlugin(cubos::engine::Cubos& cubos);
} // namespace airships::client

CUBOS_REFLECT_EXTERNAL_DECL(CUBOS_EMPTY, airships::client::ResourceInfo::ResourceTypes);