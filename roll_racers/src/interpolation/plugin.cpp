#include "plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>

#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/fixed_step/plugin.hpp>
#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/interpolation/plugin.hpp>

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

CUBOS_REFLECT_IMPL(roll_racers::InterpolatedPrev)
{
    return cubos::core::ecs::TypeBuilder<InterpolatedPrev>("roll_racers::InterpolatedPrev")
        .wrap(&InterpolatedPrev::scene);
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
    cubos.depends(cubos::engine::interpolationPlugin);

    cubos.resource<State>();

    cubos.component<InterpolatedPrev>();
    cubos.component<InterpolatedDirty>();

    cubos.relation<InterpolationOf>();

    cubos.observer("spawn Interpolated scenes")
        .onAdd<InterpolatedPrev>()
        .call([](Commands cmds, const Assets& assets,
                 Query<Entity, const InterpolatedPrev&, const Position&, const Rotation&, const Scale&> query) {
            for (auto [entity, interpolated, position, rotation, scale] : query)
            {
                auto interpolatedEnt = cmds.spawn(assets.read(interpolated.scene)->blueprint()).entity();
                cmds.relate(interpolatedEnt, entity, InterpolationOf{});
                cmds.add(interpolatedEnt, InterpolatedDirty{});
                cmds.add(interpolatedEnt, Interpolated{.currentPosition = position.vec,
                                                       .currentScale = scale.factor,
                                                       .previousPosition = position.vec,
                                                       .previousScale = scale.factor,
                                                       .nextPosition = position.vec,
                                                       .nextScale = scale.factor});
            }
        });

    cubos.system("add ChildOf to interpolated entities")
        .call([](Commands cmds,
                 Query<Entity, const InterpolatedDirty&, const InterpolationOf&, const ChildOf&,
                       const InterpolatedPrev&, Entity>
                     query,
                 Query<Entity, const InterpolationOf&> interpolationQuery) {
            for (auto [entity, dirty, interpolation, childOf, interpolated, parent] : query)
            {
                auto [interpolatedParent, interpolationOfParent] = *interpolationQuery.pin(1, parent).first();
                cmds.relate(entity, interpolatedParent, ChildOf{});
                cmds.remove<InterpolatedDirty>(entity);
            }
        });

    /**
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
    **/
}
