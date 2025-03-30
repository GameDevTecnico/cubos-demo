#include "plugin.hpp"
#include "../tile_map/plugin.hpp"
#include "../waves/plugin.hpp"
#include "../health/plugin.hpp"
#include "../destroy_tree/plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/glm.hpp>

#include <cubos/engine/transform/plugin.hpp>

#include <random>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(demo::Bullet)
{
    return cubos::core::ecs::TypeBuilder<Bullet>("demo::Bullet")
        .withField("origin", &Bullet::origin)
        .withField("target", &Bullet::target)
        .withField("baseHeight", &Bullet::baseHeight)
        .withField("maxHeight", &Bullet::maxHeight)
        .withField("speed", &Bullet::speed)
        .withField("progress", &Bullet::progress)
        .build();
}

void demo::bulletPlugin(Cubos& cubos)
{
    cubos.depends(transformPlugin);
    cubos.depends(tileMapPlugin);
    cubos.depends(wavesPlugin);
    cubos.depends(healthPlugin);
    cubos.depends(destroyTreePlugin);

    cubos.component<Bullet>();

    cubos.system("bullet movement handler")
        .before(transformUpdateTag)
        .call([](const DeltaTime& dt, Commands cmds,
                 Query<Entity, Position&, Rotation&, Bullet&, const ChildOf&, TileMap&, Waves&> query) {
            for (auto [ent, position, rotation, bullet, _2, map, waves] : query)
            {
                bullet.progress = bullet.progress + bullet.speed * dt.value();

                if (bullet.progress > 5.0)
                {
                    // Destroy bullets which fell out of the map (for quite some time already).
                    cmds.add(ent, DestroyTree{});
                    continue;
                }

                // Calculate the current position of the bullet.
                // The Y is determined from an equation of a parabola.
                auto currentXZ = glm::mix(glm::vec2(bullet.origin), glm::vec2(bullet.target), bullet.progress);
                auto currentY = bullet.baseHeight + bullet.maxHeight * (1 - glm::pow(2.0 * bullet.progress - 1.0, 2.0));
                position.vec = glm::vec3(currentXZ.x + 0.5, currentY, currentXZ.y + 0.5);

                // Rotate the bullet to face the direction of movement.
                auto dx = bullet.target.x - bullet.origin.x;
                auto dy = 8.0 - 16.0 * bullet.progress;
                auto dz = bullet.target.y - bullet.origin.y;
                rotation.quat = glm::quatLookAt(glm::normalize(glm::vec3{dx, dy, dz}), glm::vec3{0, 1, 0});

                auto tileXZ = glm::ivec2(glm::round(currentXZ));
                if (tileXZ.y >= 0 && tileXZ.y < waves.terrain.size() && tileXZ.x >= 0 &&
                    tileXZ.x < waves.terrain[tileXZ.y].size())
                {
                    // Check if the bullet has finished its path.
                    if (bullet.progress >= 1.0F)
                    {
                        auto tileEnt = map.entities[tileXZ.y][tileXZ.x];
                        if (!tileEnt.isNull())
                        {
                            cmds.add(tileEnt, Damage{.hp = 1});
                        }

                        // The bullet has reached its target.
                        cmds.add(ent, DestroyTree{});
                        continue;
                    }

                    // Check if the bullet has hit the ground or water.
                    if (bullet.progress > 0.2)
                    {
                        auto tileY = glm::max(map.tiles[tileXZ.y][tileXZ.x].blockHeight + 1.0F,
                                              waves.actual[tileXZ.y][tileXZ.x]);
                        if (currentY <= tileY)
                        {
                            cmds.add(ent, DestroyTree{});
                            continue;
                        }
                    }
                }
            }
        });
}
