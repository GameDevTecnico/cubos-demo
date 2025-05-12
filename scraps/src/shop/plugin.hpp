#pragma once

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/assets/asset.hpp>
#include <cubos/engine/prelude.hpp>
#include <cubos/engine/scene/scene.hpp>

namespace demo
{
    /// @brief Component which identifies coin entities.
    struct Coin
    {
        CUBOS_REFLECT;
    };

    /// @brief Component for shop entities.
    struct Shop
    {
        CUBOS_REFLECT;

        /// @brief Item scene.
        cubos::engine::Asset<cubos::engine::Scene> item;
    };

    void shopPlugin(cubos::engine::Cubos& cubos);
} // namespace demo
