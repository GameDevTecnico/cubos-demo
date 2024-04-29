#pragma once

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/assets/asset.hpp>
#include <cubos/engine/prelude.hpp>
#include <cubos/engine/scene/scene.hpp>

namespace demo
{
    /// @brief Component which identifies hammer tools.
    struct Hammer
    {
        CUBOS_REFLECT;
    };

    /// @brief Component which allows an object to be used to build something else.
    struct Ingredient
    {
        CUBOS_REFLECT;

        /// @brief Object to spawn in the same position as the entity when construction finishes.
        cubos::engine::Asset<cubos::engine::Scene> result;

        /// @brief Name of the root entity of the result scene.
        std::string root;
    };

    void hammerPlugin(cubos::engine::Cubos& cubos);
} // namespace demo
