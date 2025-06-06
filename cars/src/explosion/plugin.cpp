#include "plugin.hpp"
#include "explosion.hpp"
#include "particle.hpp"

#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/scene/plugin.hpp>
#include <cubos/engine/assets/plugin.hpp>

#include <glm/gtc/random.hpp>

using namespace cubos::engine;

static const Asset<Scene> ParticleAsset = AnyAsset("3651b5f5-e6be-4d8e-b0fb-1e4d6dddb44c");

void demo::explosionPlugin(Cubos& cubos)
{
    cubos.depends(assetsPlugin);
    cubos.depends(transformPlugin);

    cubos.component<Explosion>();
    cubos.component<Particle>();

    cubos.system("update explosion")
        .call([](Commands cmds, const DeltaTime& dt, Assets& assets, Query<Explosion&, const Position&> query) {
            for (auto [explosion, position] : query)
            {
                // If explosion duration is over, stop updating it.
                if (explosion.duration <= 0.0F)
                {
                    continue;
                }

                explosion.duration -= dt.value();

                // Spawn a new particle every time the timer reaches 0.
                explosion.timer -= dt.value();
                while (explosion.timer <= 0.0F)
                {
                    explosion.timer += explosion.particleTime;

                    // Pick a random direction for the particle to move in.
                    glm::vec3 velocity = glm::normalize(glm::ballRand(1.0F));
                    velocity.y = glm::abs(velocity.y);
                    velocity *= 5.0F;

                    // Spawn the particle scene and add the Particle component.
                    cmds.spawn(assets.read(ParticleAsset)->blueprint())
                        .add(Particle{.startLife = explosion.particleLife,
                                      .life = explosion.particleLife,
                                      .size = glm::linearRand(0.1F, 0.4F),
                                      .velocity = velocity})
                        .add(Position{.vec = position.vec})
                        .add(Scale{.factor = 0.0F})
                        .named("particle");
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
