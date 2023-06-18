#include "explosion.hpp"

#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/renderer/plugin.hpp>

#include <glm/gtc/random.hpp>

using cubos::core::ecs::Commands;
using cubos::core::ecs::Query;
using cubos::core::ecs::Read;
using cubos::core::ecs::Write;
using cubos::core::gl::Grid;
using namespace cubos::engine;

/// @brief Used just to store the particle grid asset.
struct ParticleHolder
{
    Asset<Grid> grid;
};

static void genGrid(Write<Assets> assets, Write<ParticleHolder> holder)
{
    Grid grid{{1, 1, 1}};
    grid.set({0, 0, 0}, 16);
    holder->grid = assets->create(std::move(grid));
}

static void explosion(Commands cmds, Query<Write<Explosion>, Read<Position>> query, Read<DeltaTime> dt,
                      Read<ParticleHolder> holder)
{
    const auto particleTime = 0.01F;
    const auto particleLife = 1.0F;

    for (auto [entity, explosion, position] : query)
    {
        explosion->life -= dt->value;
        explosion->particleTimer -= dt->value;

        while (explosion->particleTimer <= 0.0F)
        {
            explosion->particleTimer += particleTime;

            glm::vec3 velocity = glm::normalize(glm::ballRand(1.0F));
            velocity.y = glm::abs(velocity.y);
            velocity *= 50.0F;

            cmds.create()
                .add(Particle{.velocity = velocity,
                              .size = glm::linearRand(1.0F, 4.0F),
                              .startLife = particleLife,
                              .life = particleLife})
                .add(Position{.vec = position->vec})
                .add(Scale{})
                .add(RenderableGrid{holder->grid});
        }

        if (explosion->life <= 0.0F)
        {
            cmds.destroy(entity);
        }
    }
}

static void particle(Commands cmds, Query<Write<Particle>, Write<Position>, Write<Scale>> query, Read<DeltaTime> dt)
{
    for (auto [entity, particle, position, scale] : query)
    {
        particle->life -= dt->value;
        position->vec += particle->velocity * dt->value;
        particle->velocity /= 1.0 + 1.0 * dt->value;

        if (particle->life <= 0.0F)
        {
            cmds.destroy(entity);
        }
        else
        {
            scale->factor = (particle->life / particle->startLife) * particle->size;
        }
    }
}

void explosionPlugin(Cubos& cubos)
{
    cubos.addResource<ParticleHolder>();

    cubos.addComponent<Explosion>();
    cubos.addComponent<Particle>();

    cubos.startupSystem(genGrid);

    cubos.system(explosion);
    cubos.system(particle);
}
