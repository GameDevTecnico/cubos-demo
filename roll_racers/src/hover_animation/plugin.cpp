#include "plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/transform/plugin.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(roll_racers::HoverAnimation)
{
    return cubos::core::ecs::TypeBuilder<HoverAnimation>("roll_racers::HoverAnimation")
        .withField("rotationPeriod", &HoverAnimation::rotationPeriod)
        .withField("translationPeriod", &HoverAnimation::translationPeriod)
        .withField("translationDistance", &HoverAnimation::translationDistance)
        .withField("timeAcc", &HoverAnimation::timeAcc)
        .build();
}

void roll_racers::hoverAnimationPlugin(Cubos& cubos)
{
    cubos.depends(transformPlugin);

    cubos.component<HoverAnimation>();

    cubos.system("do HoverAnimation").call([](Query<Position&, Rotation&, HoverAnimation&> query, const DeltaTime& dt) {
        for (auto [position, rotation, anim] : query)
        {
            anim.timeAcc += dt.value();
            rotation.quat = glm::angleAxis(anim.timeAcc * 2.0F * glm::pi<float>() / anim.rotationPeriod,
                                           glm::vec3(0.0F, 1.0F, 0.0F));
            position.vec.y =
                anim.translationDistance * glm::sin(anim.timeAcc * 2.0F * glm::pi<float>() / anim.translationPeriod);
        }
    });
}