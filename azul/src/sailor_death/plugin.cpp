#include "plugin.hpp"
#include "../destroy_tree/plugin.hpp"
#include "../health/plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/string.hpp>

#include <cubos/engine/prelude.hpp>
#include <cubos/engine/transform/plugin.hpp>

#include <random>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(demo::SailorDeath)
{
    return cubos::core::ecs::TypeBuilder<SailorDeath>("demo::SailorDeath").withField("hp", &SailorDeath::hp).build();
}

CUBOS_REFLECT_IMPL(demo::SailorDeathAnimation)
{
    return cubos::core::ecs::TypeBuilder<SailorDeathAnimation>("demo::SailorDeathAnimation")
        .withField("time", &SailorDeathAnimation::time)
        .withField("speed", &SailorDeathAnimation::speed)
        .withField("duration", &SailorDeathAnimation::duration)
        .withField("source", &SailorDeathAnimation::source)
        .withField("target", &SailorDeathAnimation::target)
        .withField("maxHeight", &SailorDeathAnimation::maxHeight)
        .build();
}

void demo::sailorDeathPlugin(Cubos& cubos)
{
    cubos.depends(healthPlugin);
    cubos.depends(destroyTreePlugin);
    cubos.depends(transformPlugin);

    cubos.relation<SailorDeath>();
    cubos.component<SailorDeathAnimation>();

    cubos.system("kill sailors")
        .call([](Commands cmds,
                 Query<Entity, Position&, Rotation&, Scale&, LocalToWorld&, const SailorDeath&, const Health&, Entity>
                     query,
                 Query<const ChildOf&, Entity> parentQuery) {
            for (auto [ent, pos, rot, scale, ltw, death, health, playerEnt] : query)
            {
                if (health.hp <= death.hp)
                {
                    auto parent = parentQuery.pin(0, ent).first();
                    auto [_1, parentEnt] = *parent;
                    pos.vec = ltw.worldPosition();
                    rot.quat = ltw.worldRotation();
                    scale.factor = ltw.worldScale();

                    auto rightDir = rot.quat * glm::vec3{1.0F, 0.0F, 0.0F};
                    auto sign = (rand() % 2) ? -1.0F : 1.0F;

                    auto source = glm::vec2(pos.vec.x, pos.vec.z);
                    auto target = source + glm::vec2{rightDir.x, rightDir.z} * sign;

                    cmds.unrelate<ChildOf>(ent, parentEnt);
                    cmds.unrelate<SailorDeath>(ent, playerEnt);
                    cmds.add(ent, SailorDeathAnimation{
                                      .source = source,
                                      .target = target,
                                      .initialHeight = pos.vec.y,
                                  });
                }
            }
        });

    cubos.system("do sailor death animation")
        .before(transformUpdateTag)
        .call([](Commands cmds, const DeltaTime& dt, Query<Entity, Position&, SailorDeathAnimation&> query) {
            for (auto [ent, pos, anim] : query)
            {
                anim.time += dt.value() * anim.speed;
                if (anim.time >= anim.duration)
                {
                    cmds.add(ent, DestroyTree{});
                }

                glm::vec2 posXZ = glm::mix(anim.source, anim.target, anim.time);
                auto currentY = anim.initialHeight + anim.maxHeight * (1 - glm::pow(2.0 * anim.time - 1.0, 2.0));
                pos.vec = {posXZ.x, currentY, posXZ.y};
            }
        });
}
