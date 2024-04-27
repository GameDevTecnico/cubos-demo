#include "plugin.hpp"
#include "checkpoint.hpp"
#include "obstacle.hpp"
#include "racer.hpp"
#include "../dead/dead.hpp"
#include "../dead/plugin.hpp"
#include "../explosion/explosion.hpp"
#include "../explosion/plugin.hpp"

#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/renderer/plugin.hpp>
#include <cubos/engine/collisions/colliding_with.hpp>
#include <cubos/engine/collisions/plugin.hpp>

using namespace cubos::engine;

static CUBOS_DEFINE_TAG(raceKill);

void demo::racePlugin(Cubos& cubos)
{
    cubos.depends(deadPlugin);
    cubos.depends(explosionPlugin);
    cubos.depends(transformPlugin);
    cubos.depends(collisionsPlugin);

    cubos.component<Checkpoint>();
    cubos.component<Obstacle>();
    cubos.component<Racer>();

    cubos.tag(raceKill);

    cubos.system("increase racers lap time").call([](const DeltaTime& dt, Query<Racer&> query) {
        for (auto [racer] : query)
        {
            if (!racer.currentCheckpoint.isNull())
            {
                racer.currentLapTime += dt.value();
            }
        }
    });

    cubos.system("update racers checkpoints on checkpoint collision")
        .with<Racer>()
        .related<CollidingWith>()
        .entity()
        .with<Checkpoint>()
        .call([](Query<Racer&, Entity, const Checkpoint&> query) {
            for (auto [racer, checkpointEnt, checkpoint] : query)
            {
                if (racer.currentCheckpoint.isNull())
                {
                    // If the racer hasn't passed any checkpoints yet, then check if the current checkpoint is the
                    // finish line.
                    if (checkpoint.isFinishLine)
                    {
                        // The racer is starting a lap.
                        racer.currentLapTime = 0.0F;
                        racer.currentCheckpoint = checkpointEnt;
                    }
                }
                else if (racer.currentCheckpoint == checkpoint.previous)
                {
                    // The racer has passed the previous checkpoint, so update the current checkpoint.
                    racer.currentCheckpoint = checkpointEnt;
                    if (checkpoint.isFinishLine)
                    {
                        // The racer has finished a lap.
                        racer.lapTimes.push_back(racer.currentLapTime);
                        racer.currentLapTime = 0.0F;
                        racer.successiveLaps += 1;
                        racer.maxSuccessiveLaps = glm::max(racer.maxSuccessiveLaps, racer.successiveLaps);
                    }
                }
            }
        });

    cubos.system("kill racers on obstacle collision")
        .tagged(raceKill)
        .entity()
        .with<Racer>()
        .without<Dead>()
        .related<CollidingWith>()
        .with<Obstacle>()
        .call([](Commands cmds, Query<Entity> query) {
            for (auto [ent] : query)
            {
                cmds.add(ent, Dead{});
                cmds.add(ent, Explosion{});
            }
        });

    cubos.system("reset racers after explosion is over")
        .entity()
        .after(raceKill)
        .with<Dead>()
        .call([](Commands cmds, Query<Entity, Racer&, Position&, Rotation&, Explosion&> query) {
            for (auto [ent, racer, position, rotation, explosion] : query)
            {
                if (explosion.duration <= 0.0F)
                {
                    // Move the racer back to the starting position and rotation.
                    position.vec = racer.startingPosition;
                    rotation.quat = racer.startingRotation;

                    // Reset the current checkpoint and lap time.
                    racer.currentCheckpoint = {};
                    racer.currentLapTime = 0.0F;
                    racer.successiveLaps = 0;

                    // Remove the Dead and Explosion components.
                    cmds.remove<Dead>(ent);
                    cmds.remove<Explosion>(ent);
                }
            }
        });
}
