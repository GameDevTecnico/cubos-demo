#include "plugin.hpp"
#include "../health/health.hpp"
#include "../health/plugin.hpp"
#include <cubos/engine/collisions/colliding_with.hpp>
#include <cubos/engine/collisions/plugin.hpp>
#include <cubos/engine/scene/scene.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/assets/plugin.hpp>

using namespace cubos::engine;

static const Asset<Scene> ExplosionSceneAsset = AnyAsset("a1a8d7bc-8d45-4aa2-ab53-462bf7b6733b");

void bulletPlugin(Cubos& cubos)
{
    cubos.depends(assetsPlugin);
    cubos.depends(transformPlugin);
    cubos.depends(collisionsPlugin);
    cubos.depends(healthPlugin);

    cubos.component<Bullet>();

    cubos.system("hit entity")
        .after(collisionsTag)
        .call([](Commands cmds, const Assets& assets,
                 Query<Entity, Bullet&, const Position&, const CollidingWith&, Entity, Opt<Health&>> query) {
            for (auto [ent1, bullet, position, colliding, ent2, health] : query)
            {
                if (health.contains()) // it's a wall
                {
                    health.value().points -= bullet.damage;
                }

                // add explosion
                auto explosionBuilder = cmds.spawn(assets.read(ExplosionSceneAsset)->blueprint);
                explosionBuilder.add("explosion", Position{.vec = position.vec});

                cmds.destroy(ent1);
            }
        });

    cubos.system("bullet timeout").call([](Commands cmds, const DeltaTime& dt, Query<Entity, Bullet&> query) {
        for (auto [ent, bullet] : query)
        {
            if (bullet.time > 10.0F)
            {
                cmds.destroy(ent);
            }
            bullet.time += dt.value();
        }
    });
}