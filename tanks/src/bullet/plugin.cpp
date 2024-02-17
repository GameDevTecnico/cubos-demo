#include "plugin.hpp"
#include "../health/health.hpp"
#include <cubos/engine/collisions/colliding_with.hpp>

using namespace cubos::engine;

void bulletPlugin(Cubos& cubos)
{
    cubos.addComponent<Bullet>();

    cubos.system("hit entity")
        .after("cubos.collisions.narrow")
        .call([](Commands cmds, Query<Entity, Bullet&, CollidingWith&, Entity, Opt<Health&>> query) {
            for (auto [ent1, bullet, colliding, ent2, health] : query)
            {
                if (health.contains()) // it's a wall
                {
                    health.value().points -= bullet.damage;
                }
                cmds.destroy(ent1);
            }
        });

    cubos.system("bullet timeout").call([](Commands cmds, DeltaTime& dt, Query<Entity, Bullet&> query) {
        for (auto [ent, bullet] : query)
        {
            if (bullet.time > 10.0F)
            {
                cmds.destroy(ent);
            }
            bullet.time += dt.value;
        }
    });
}