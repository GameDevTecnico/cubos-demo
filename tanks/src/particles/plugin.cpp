#include "plugin.hpp"
#include "gun_shoot_explosion.hpp"
#include "bullet_hit_explosion.hpp"
#include "particle.hpp"

#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/scene/plugin.hpp>

#include <glm/gtc/random.hpp>

using namespace cubos::engine;

static const Asset<Scene> ParticleAsset = AnyAsset("3651b5f5-e6be-4d8e-b0fb-1e4d6dddb44c");

void explosionPlugin(Cubos& cubos)
{
    cubos.depends(transformPlugin);
    cubos.depends(assetsPlugin);

    cubos.component<GunShootExplosion>();
    cubos.component<BulletHitExplosion>();
    cubos.component<Particle>();

    // system for managing the effects when a bullet hits a target
    // maybe different color according to tank or wall
    cubos.system("update bullet explosion")
        .call([](Commands cmds, const DeltaTime& dt, Assets& assets,
                 Query<Entity, BulletHitExplosion&, const Position&> query) {
            for (auto [ent, explosion, position] : query)
            {
                // If explosion duration is over, stop updating it.
                if (explosion.duration <= 0.0F)
                {
                    cmds.destroy(ent);
                    continue;
                }

                explosion.duration -= dt.value();

                // Spawn a new particle every time the timer reaches 0.
                explosion.timer -= dt.value();
                auto readAsset = assets.read(ParticleAsset);
                while (explosion.timer <= 0.0F)
                {
                    explosion.timer += explosion.particleTime;

                    // Pick a random direction for the particle to move in.
                    glm::vec3 velocity = glm::normalize(glm::ballRand(1.0F));
                    velocity *= 5.0F;

                    // Spawn the particle scene and add the Particle component.
                    cmds.spawn(readAsset->blueprint)
                        .add("particle", Particle{.startLife = explosion.particleLife,
                                                  .life = explosion.particleLife,
                                                  .size = glm::linearRand(0.05F, 0.2F),
                                                  .velocity = velocity})
                        .add("particle", Position{.vec = position.vec})
                        .add("particle", Scale{.factor = 0.0F});
                }
            }
        });

    // system for managing the gun explosion effects
    cubos.system("update gun explosion")
        .call([](Commands cmds, const DeltaTime& dt, Assets& assets,
                 Query<Entity, GunShootExplosion&, const Position&, const Rotation&> query) {
            for (auto [ent, explosion, position, rotation] : query)
            {
                // If explosion duration is over, stop updating it.
                if (explosion.duration <= 0.0F)
                {
                    cmds.destroy(ent);
                    continue;
                }

                explosion.duration -= dt.value();

                // Spawn a new particle every time the timer reaches 0.
                explosion.timer -= dt.value();
                auto readAsset = assets.read(ParticleAsset);
                while (explosion.timer <= 0.0F)
                {
                    explosion.timer += explosion.particleTime;

                    // Pick a random direction for the particle to move in.
                    glm::vec3 velocity = glm::normalize(glm::ballRand(1.0F));
                    velocity.z = glm::abs(velocity.z);
                    if (glm::dot(glm::vec3(0.0F, 0.0F, 1.0F), velocity) < 0.93)
                    {
                        continue;
                    }
                    velocity *= 10.0F;

                    // Spawn the particle scene and add the Particle component.
                    cmds.spawn(readAsset->blueprint)
                        .add("particle", Particle{.startLife = explosion.particleLife,
                                                  .life = explosion.particleLife,
                                                  .size = glm::linearRand(0.05F, 0.2F),
                                                  .velocity = rotation.quat * velocity})
                        .add("particle", Position{.vec = position.vec})
                        .add("particle", Scale{.factor = 0.0F});
                }
            }
        });

    cubos.system("update explosion particles")
        .call([](Commands cmds, const DeltaTime& dt, Query<Entity, Particle&, Position&, Scale&> query) {
            for (auto [ent, particle, position, scale] : query)
            {
                particle.life -= dt.value();
                if (particle.life <= 0.0F)
                {
                    // If particle life is over, destroy its entity.
                    cmds.destroy(ent);
                    continue;
                }

                position.vec += particle.velocity * dt.value();
                particle.velocity /= 1.0 + 1.0 * dt.value();
                scale.factor = (particle.life / particle.startLife) * particle.size;
            }
        });
}
