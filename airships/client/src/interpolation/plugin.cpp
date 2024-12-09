#include "plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>

#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/fixed_step/plugin.hpp>
#include <cubos/engine/assets/plugin.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(airships::client::InterpolationOf)
{
    return cubos::core::ecs::TypeBuilder<InterpolationOf>("airships::client::InterpolationOf")
        .withField("previousPosition", &InterpolationOf::previousPosition)
        .withField("previousRotation", &InterpolationOf::previousRotation)
        .withField("previousScale", &InterpolationOf::previousScale)
        .tree()
        .build();
}

CUBOS_REFLECT_IMPL(airships::client::Interpolated)
{
    return cubos::core::ecs::TypeBuilder<Interpolated>("airships::client::Interpolated")
        .withField("scene", &Interpolated::scene)
        .build();
}

namespace
{
    struct State
    {
        CUBOS_ANONYMOUS_REFLECT(State);

        bool stepped = false;
    };

    struct InterpolatedDirty
    {
        CUBOS_REFLECT;
    };

    CUBOS_REFLECT_IMPL(InterpolatedDirty)
    {
        return cubos::core::ecs::TypeBuilder<InterpolatedDirty>("airships::client::InterpolatedDirty").build();
    }
} // namespace

void airships::client::interpolationPlugin(Cubos& cubos)
{
    cubos.depends(transformPlugin);
    cubos.depends(fixedStepPlugin);
    cubos.depends(assetsPlugin);

    cubos.resource<State>();

    cubos.component<Interpolated>();
    cubos.component<InterpolatedDirty>();

    cubos.relation<InterpolationOf>();

    cubos.observer("spawn Interpolated scenes")
        .onAdd<Interpolated>()
        .call([](Commands cmds, const Assets& assets, Query<Entity, const Interpolated&> query) {
            for (auto [entity, interpolated] : query)
            {
                auto scene = assets.read(interpolated.scene);
                auto interpolatedEnt = cmds.spawn(scene->blueprint).entity("root");
                cmds.relate(interpolatedEnt, entity, InterpolationOf{});
                cmds.add(interpolatedEnt, InterpolatedDirty{});
            }
        });

    cubos.system("add ChildOf to interpolated entities")
        .call([](Commands cmds,
                 Query<Entity, const InterpolatedDirty&, const InterpolationOf&, const ChildOf&, const Interpolated&,
                       Entity>
                     query,
                 Query<Entity, const InterpolationOf&> interpolationQuery) {
            for (auto [entity, dirty, interpolation, childOf, interpolated, parent] : query)
            {
                auto [interpolatedParent, interpolationOfParent] = *interpolationQuery.pin(1, parent).first();
                cmds.relate(entity, interpolatedParent, ChildOf{});
                cmds.remove<InterpolatedDirty>(entity);
            }
        });

    cubos.system("do interpolation on InterpolationOf sources")
        .before(fixedStepTag)
        .call([](const FixedAccumulatedTime& acc, const DeltaTime& dt, const FixedDeltaTime& fdt,
                 Query<Position&, Rotation&, Scale&, const InterpolationOf&> query) {
            float alpha = (acc.value + dt.value()) / fdt.value;

            for (auto [position, rotation, scale, interpolation] : query)
            {
                if (interpolation.previousScale == 0.0F)
                {
                    continue;
                }

                position.vec = glm::mix(interpolation.previousPosition, interpolation.nextPosition, alpha);
                rotation.quat = glm::slerp(interpolation.previousRotation, interpolation.nextRotation, alpha);
                scale.factor = glm::mix(interpolation.previousScale, interpolation.nextScale, alpha);
            }
        });

    cubos.system("detect fixedStep for InterpolationOf").tagged(fixedStepTag).call([](State& state) {
        state.stepped = true;
    });

    cubos.system("update InterpolationOf relations from targets")
        .after(fixedStepTag)
        .call([](State& state, Query<InterpolationOf&, const Position&, const Rotation&, const Scale&> query) {
            if (state.stepped)
            {
                state.stepped = false;

                for (auto [interpolation, position, rotation, scale] : query)
                {
                    interpolation.previousPosition = interpolation.nextPosition;
                    interpolation.previousRotation = interpolation.nextRotation;
                    interpolation.previousScale = interpolation.nextScale;
                    interpolation.nextPosition = position.vec;
                    interpolation.nextRotation = rotation.quat;
                    interpolation.nextScale = scale.factor;
                }
            }
        });
}
