#include "plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/prelude.hpp>
#include <cubos/engine/transform/plugin.hpp>

#include <random>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(demo::RotatingAnimation)
{
    return cubos::core::ecs::TypeBuilder<RotatingAnimation>("demo::RotatingAnimation")
        .withField("baseHeight", &RotatingAnimation::baseHeight)
        .withField("time", &RotatingAnimation::time)
        .withField("speed", &RotatingAnimation::speed)
        .build();
}

void demo::rotatingAnimationPlugin(Cubos& cubos)
{
    cubos.depends(transformPlugin);

    cubos.component<RotatingAnimation>();

    cubos.observer("setup rotating animation")
        .onAdd<RotatingAnimation>()
        .call([](Query<Entity, const Position&, RotatingAnimation&> query) {
            for (auto [ent, pos, anim] : query)
            {
                anim.baseHeight = pos.vec.y;
            }
        });

    cubos.system("do rotating animation")
        .before(transformUpdateTag)
        .call([](const DeltaTime& dt, Query<Entity, Position&, Rotation&, RotatingAnimation&> query) {
            for (auto [ent, pos, rot, anim] : query)
            {
                anim.time += dt.value() * anim.speed;
                pos.vec.y = anim.baseHeight + std::sin(anim.time * glm::two_pi<float>()) * 0.25F;
                rot.quat = glm::angleAxis(anim.time * glm::two_pi<float>(), glm::vec3{0.0F, 1.0F, 0.0F});
            }
        });
}
