#include "plugin.hpp"
#include "../interaction/plugin.hpp"
#include "../object/plugin.hpp"
#include "../zombie/plugin.hpp"
#include "../health/plugin.hpp"
#include "../tile_map/plugin.hpp"
#include "cubos/engine/prelude.hpp"

#include <random>

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/collisions/plugin.hpp>
#include <cubos/engine/collisions/colliding_with.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(demo::Turret)
{
    return cubos::core::ecs::TypeBuilder<Turret>("demo::Turret")
        .withField("bullet", &Turret::bullet)
        .withField("root", &Turret::root)
        .withField("range", &Turret::range)
        .withField("cooldown", &Turret::cooldown)
        .withField("timeSinceLastShot", &Turret::timeSinceLastShot)
        .withField("rotationTime", &Turret::rotationTime)
        .withField("bulletHeight", &Turret::bulletHeight)
        .withField("bulletDistance", &Turret::bulletDistance)
        .withField("bulletSpeed", &Turret::bulletSpeed)
        .withField("maxBulletTime", &Turret::maxBulletTime)
        .withField("ammo", &Turret::ammo)
        .withField("maxAmmoForReload", &Turret::maxAmmoForReload)
        .withField("ammoPerReload", &Turret::ammoPerReload)
        .build();
}

CUBOS_REFLECT_IMPL(demo::Ammo)
{
    return cubos::core::ecs::TypeBuilder<Ammo>("demo::Ammo").build();
}

CUBOS_REFLECT_IMPL(demo::Bullet)
{
    return cubos::core::ecs::TypeBuilder<Bullet>("demo::Bullet")
        .withField("shooter", &Bullet::shooter)
        .withField("speed", &Bullet::speed)
        .withField("time", &Bullet::time)
        .withField("maxTime", &Bullet::maxTime)
        .withField("team", &Bullet::team)
        .build();
}

void demo::turretPlugin(Cubos& cubos)
{
    cubos.depends(assetsPlugin);
    cubos.depends(transformPlugin);
    cubos.depends(collisionsPlugin);
    cubos.depends(interactionPlugin);
    cubos.depends(zombiePlugin);
    cubos.depends(tileMapPlugin);
    cubos.depends(healthPlugin);

    cubos.component<Turret>();
    cubos.component<Ammo>();
    cubos.component<Bullet>();

    cubos.observer("remove Turret gun when destroyed")
        .entity()
        .related<ChildOf>()
        .onRemove<Turret>()
        .call([](Commands cmds, Query<Entity> children) {
            for (auto [child] : children)
            {
                cmds.destroy(child);
            }
        });

    cubos.observer("reload Turret ammo when interacted with Ammo")
        .onAdd<Interaction>()
        .call([](Commands cmds, Query<const Interaction&, Turret&> turrets,
                 Query<Entity, const Ammo&, const ChildOf&> ammo) {
            for (auto [interaction, turret] : turrets)
            {
                for (auto [entity, ammo, childOf] : ammo.pin(1, interaction.entity))
                {
                    if (turret.ammo < turret.maxAmmoForReload)
                    {
                        cmds.destroy(entity);
                        turret.ammo += turret.ammoPerReload;
                    }
                }
            }
        });

    cubos.system("aim and shoot Turret")
        .with<ZombieController>()
        .call([](Commands cmds, Assets& assets, DeltaTime& dt, Query<const Position&> zombies,
                 Query<Rotation&, const ChildOf&, const Position&, const Health&, Turret&, Entity> turrets) {
            for (auto [gunRotation, _, turretPosition, health, turret, turretEnt] : turrets)
            {
                turret.timeSinceLastShot += dt.value();

                if (turret.ammo == 0)
                {
                    continue;
                }

                // Select the closest zombie.
                float closestZombieDistance2 = INFINITY;
                glm::vec2 closestZombiePosition{INFINITY, INFINITY};

                for (auto [zombiePosition] : zombies)
                {
                    float distance2 = glm::distance2(turretPosition.vec, zombiePosition.vec);
                    if (distance2 < closestZombieDistance2)
                    {
                        closestZombieDistance2 = distance2;
                        closestZombiePosition = {zombiePosition.vec.x, zombiePosition.vec.z};
                    }
                }

                glm::vec2 turretPosition2D{turretPosition.vec.x, turretPosition.vec.z};
                if (closestZombieDistance2 == INFINITY || closestZombiePosition == turretPosition2D ||
                    closestZombieDistance2 > turret.range * turret.range)
                {
                    continue;
                }

                // Figure out the target rotation of the gun.
                glm::vec2 targetDirection = glm::normalize(closestZombiePosition - turretPosition2D);
                float targetAngle = std::atan2(-targetDirection.y, targetDirection.x);
                auto targetGunRotation = glm::angleAxis(targetAngle, glm::vec3{0.0F, 1.0F, 0.0F});

                // Smoothly rotate the gun towards the target.
                auto rotationSpeed = dt.value() * glm::radians(180.0F) / turret.rotationTime;
                auto missingRotation = glm::inverse(gunRotation.quat) * targetGunRotation;
                if (glm::angle(missingRotation) > rotationSpeed)
                {
                    // Rotate through the shortest path.
                    gunRotation.quat =
                        glm::slerp(gunRotation.quat, targetGunRotation, rotationSpeed / glm::angle(missingRotation));
                }
                else
                {
                    // The gun is pointing at the target. Shoot if the cooldown has passed.
                    gunRotation.quat = targetGunRotation;

                    if (turret.timeSinceLastShot >= turret.cooldown)
                    {
                        turret.timeSinceLastShot = 0.0F;
                        turret.ammo -= 1;

                        auto bulletPosition = turretPosition.vec + glm::vec3{0.0F, turret.bulletHeight, 0.0F};
                        bulletPosition += targetGunRotation * glm::vec3{turret.bulletDistance, 0.0F, 0.0F};

                        // Instantiate the bullet scene.
                        cmds.spawn(assets.read(turret.bullet)->blueprint)
                            .add(turret.root, Position{bulletPosition})
                            .add(turret.root, Rotation{targetGunRotation})
                            .add(turret.root, Bullet{
                                                  .shooter = turretEnt,
                                                  .speed = turret.bulletSpeed,
                                                  .maxTime = turret.maxBulletTime,
                                                  .team = health.team,
                                              });
                    }
                }
            }
        });

    cubos.system("update Bullets")
        .call([](Commands cmds, const DeltaTime& dt, Query<Entity, Position&, const Rotation&, Bullet&> bullets,
                 Query<TileMap&> maps, Query<const CollidingWith&, Entity, Opt<Health&>> collidingWith) {
            if (maps.empty())
            {
                return;
            }

            auto [map] = *maps.first();

            for (auto [ent, position, rotation, bullet] : bullets)
            {
                bullet.time += dt.value();
                if (bullet.time >= bullet.maxTime)
                {
                    cmds.destroy(ent);
                    continue;
                }

                // Move the bullet forward.
                position.vec += rotation.quat * glm::vec3{bullet.speed, 0.0F, 0.0F} * dt.value();

                for (auto [_, hit, health] : collidingWith.pin(0, ent))
                {
                    if (hit == bullet.shooter)
                    {
                        continue;
                    }

                    if (health.contains())
                    {
                        if (health->team != bullet.team)
                        {
                            health->hp--;
                        }
                    }
                    cmds.destroy(ent);
                    break;
                }
            }
        });
}
