#include "plugin.hpp"
#include "../damageable/plugin.hpp"
#include "../balloons/plugin.hpp"
#include "../destroy_tree/plugin.hpp"

#include <cubos/engine/collisions/plugin.hpp>
#include <cubos/engine/collisions/colliding_with.hpp>
#include <cubos/core/ecs/reflection.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(airships::client::Bullet)
{
    return cubos::core::ecs::TypeBuilder<Bullet>("airships::client::Bullet").build();
}

void airships::client::bulletPlugin(Cubos& cubos)
{
    cubos.depends(collisionsPlugin);
    cubos.depends(damageablePlugin);
    cubos.depends(destroyTreePlugin);
    cubos.depends(transformPlugin);
    cubos.depends(balloonsPlugin);

    cubos.component<Bullet>();

    cubos.system("handle bullet collisions")
        .call([](Commands cmds, Query<Entity, const Bullet&, const CollidingWith&, Damageable&> query) {
            for (auto [entity, bullet, collidingWith, damageable] : query)
            {
                damageable.health -= 1;
                cmds.add(entity, DestroyTree{});
            }
        });

    cubos.system("handle bullet collisions with balloons")
        .call([](Commands cmds, Query<Entity, const Bullet&, const CollidingWith&, Entity, BalloonInfo&> query) {
            for (auto [entity, bullet, collidingWith, entBalloon, balloon] : query)
            {
                cmds.add(entBalloon, DestroyTree{});
                cmds.add(entity, DestroyTree{});
            }
        });

    cubos.system("destroy bullets which are too low")
        .call([](Commands cmds, Query<Entity, const Bullet&, const Position&> query) {
            for (auto [entity, bullet, position] : query)
            {
                if (position.vec.y < -1000.0F)
                {
                    cmds.add(entity, DestroyTree{});
                }
            }
        });
}
