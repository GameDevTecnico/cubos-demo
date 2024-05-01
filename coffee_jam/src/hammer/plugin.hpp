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

    /// @brief Component for recipe entities.
    struct Recipe
    {
        CUBOS_REFLECT;

        /// @brief Result scene.
        cubos::engine::Asset<cubos::engine::Scene> result;

        /// @brief Scene root entity name.
        std::string root;
    };

    /// @brief Component used to describe ingredient types.
    struct Ingredient
    {
        CUBOS_REFLECT;

        /// @brief Ingredient scene.
        cubos::engine::Asset<cubos::engine::Scene> scene;

        /// @brief Scene root entity name.
        std::string root;
    };

    /// @brief Component which when added replaces itself by a IsIngredient relation to the Ingredient with the same
    /// scene asset handle.
    ///
    /// This hack is required because we can't link the ingredient type entities directly in the ingredient scenes.
    struct IsIngredientAsset
    {
        CUBOS_REFLECT;

        /// @brief Ingredient scene.
        cubos::engine::Asset<cubos::engine::Scene> scene;
    };

    /// @brief Relation between an entity and the ingredient it represents.
    struct IsIngredient
    {
        CUBOS_REFLECT;
    };

    /// @brief Relation between a recipe and the ingredients it requires.
    struct Requires
    {
        CUBOS_REFLECT;

        /// @brief Amount of the ingredient required.
        int amount{1};
    };

    /// @brief Relation between a constructed entity and the recipe that produced it.
    struct ConstructedFrom
    {
        CUBOS_REFLECT;
    };

    void hammerPlugin(cubos::engine::Cubos& cubos);
} // namespace demo
