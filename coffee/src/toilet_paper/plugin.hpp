#pragma once

#include <cubos/engine/prelude.hpp>
#include <cubos/engine/scene/scene.hpp>
#include <cubos/engine/assets/asset.hpp>
#include <cubos/engine/physics/constraints/distance_constraint.hpp>

namespace coffee
{
    struct ToiletPaper
    {
        CUBOS_REFLECT;

        bool attached = false;
        cubos::engine::DistanceConstraint distanceConstraint{};
        float gravity{};
    };

    struct SpawnOrphan
    {
        CUBOS_REFLECT;

        cubos::engine::Asset<cubos::engine::Scene> scene;
        std::string entity;
    };

    void toiletPaperPlugin(cubos::engine::Cubos& cubos);
} // namespace coffee