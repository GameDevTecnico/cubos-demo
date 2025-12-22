#include "plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>

#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/fixed_step/plugin.hpp>
#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/interpolation/plugin.hpp>
#include <cubos/engine/physics/plugin.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(roll_racers::InterpolationOf)
{
    return cubos::core::ecs::TypeBuilder<InterpolationOf>("roll_racers::InterpolationOf")
        .withField("previousPosition", &InterpolationOf::previousPosition)
        .withField("previousRotation", &InterpolationOf::previousRotation)
        .withField("previousScale", &InterpolationOf::previousScale)
        .tree()
        .build();
}

CUBOS_REFLECT_IMPL(roll_racers::InterpolatedFlag)
{
    return cubos::core::ecs::TypeBuilder<InterpolatedFlag>("roll_racers::InterpolatedFlag")
        .wrap(&InterpolatedFlag::scene);
}

CUBOS_REFLECT_IMPL(roll_racers::InterpolatedDirty)
{
    return cubos::core::ecs::TypeBuilder<InterpolatedDirty>("roll_racers::InterpolatedDirty").build();
}

namespace
{
    struct State
    {
        CUBOS_ANONYMOUS_REFLECT(State);

        bool stepped = false;
    };
} // namespace

void roll_racers::interpolationPlugin(Cubos& cubos)
{
    cubos.depends(transformPlugin);
    cubos.depends(fixedStepPlugin);
    cubos.depends(assetsPlugin);
    cubos.depends(physicsPlugin);
    cubos.depends(cubos::engine::interpolationPlugin);

    cubos.resource<State>();

    cubos.component<InterpolatedFlag>();
    cubos.component<InterpolatedDirty>();

    cubos.relation<InterpolationOf>();

    cubos.observer("spawn Interpolated scenes")
        .onAdd<InterpolatedFlag>()
        .call([](Commands cmds, const Assets& assets, Query<Entity, const InterpolatedFlag&> query) {
            for (auto [entity, interpolated] : query)
            {
                auto interpolatedEnt = cmds.spawn(assets.read(interpolated.scene)->blueprint()).entity();
                cmds.relate(interpolatedEnt, entity, InterpolationOf{});
                cmds.add(interpolatedEnt, InterpolatedDirty{});
            }
        });

    cubos.system("add ChildOf to interpolated entities")
        .call([](Commands cmds,
                 Query<Entity, const InterpolatedDirty&, const InterpolationOf&, const ChildOf&,
                       const InterpolatedFlag&, Entity>
                     query,
                 Query<Entity, const InterpolationOf&> interpolationQuery) {
            for (auto [entity, dirty, interpolation, childOf, interpolated, parent] : query)
            {
                auto [interpolatedParent, interpolationOfParent] = *interpolationQuery.pin(1, parent).first();
                cmds.relate(entity, interpolatedParent, ChildOf{});
                cmds.remove<InterpolatedDirty>(entity);
            }
        });

    cubos.system("detect fixedStep for InterpolationOf").tagged(fixedStepTag).call([](State& state) {
        state.stepped = true;
    });

    cubos.system("update interpolated of interpolated targets (hack)")
        .after(fixedStepTag)
        .call([](State& state, Query<Position&, Rotation&, Scale&, Interpolated&, InterpolationOf&, const Position&,
                                     const Rotation&, const Scale&>
                                   query) {
            if (state.stepped)
            {
                state.stepped = false;

                for (auto [position1, rotation1, scale1, interpolated, interpolation, position2, rotation2, scale2] :
                     query)
                {
                    interpolated.nextPosition = position2.vec;
                    interpolated.currentPosition = position1.vec;
                    /* we have no intepolation of rotation and scale yet */
                    rotation1.quat = rotation2.quat;
                    scale1.factor = scale2.factor;
                }
            }
        });

    /*
    cubos.system("do interpolation on InterpolationOf sources")
        .before(fixedStepTag)
        .call([](const FixedAccumulatedTime& acc, const DeltaTime& dt, const FixedDeltaTime& fdt,
                 Query<Position&, Rotation&, Scale&, const InterpolationOf&> query) {
            float alpha = (acc.value + dt.value()) / fdt.value;
            alpha = glm::clamp(alpha, 0.0F, 1.0F);

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
    */
}
