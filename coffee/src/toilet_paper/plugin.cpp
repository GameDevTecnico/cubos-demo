#include "plugin.hpp"
#include "../car/plugin.hpp"
#include "../interpolation/plugin.hpp"
#include "../score/plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/glm.hpp>

#include <cubos/engine/collisions/colliding_with.hpp>
#include <cubos/engine/collisions/plugin.hpp>
#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/transform/child_of.hpp>
#include <cubos/engine/transform/local_to_world.hpp>
#include <cubos/engine/transform/position.hpp>
#include <cubos/engine/audio/plugin.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(coffee::ToiletPaper)
{
    return cubos::core::ecs::TypeBuilder<ToiletPaper>("coffee::ToiletPaper")
        .withField("player", &ToiletPaper::player)
        .withField("carPosition", &ToiletPaper::carPosition)
        .withField("immunityTime", &ToiletPaper::immunityTime)
        .withField("heldTime", &ToiletPaper::heldTime)
        .build();
}

void coffee::toiletPaperPlugin(Cubos& cubos)
{
    cubos.depends(transformPlugin);
    cubos.depends(collisionsPlugin);
    cubos.depends(coffee::interpolationPlugin);
    cubos.depends(coffee::carPlugin);
    cubos.depends(coffee::scorePlugin);

    cubos.component<ToiletPaper>();

    cubos.system("pick up Toilet paper")
        .after(transformUpdateTag)
        .call([](Commands commands, Query<Entity, const PlayerOwner&, const CollidingWith&, Entity, ToiletPaper&> query,
                 PlayerScores& scores) {
            for (auto [ent1, carOwner, collidingWith, ent2, toiletPaper] : query)
            {
                if (toiletPaper.player == -1)
                {
                    toiletPaper.player = carOwner.player;
                    scores.scores[toiletPaper.player - 1] += 100;
                    commands.relate(ent2, ent1, ChildOf{});
                    commands.add(ent2, Position{toiletPaper.carPosition});

                    AudioSource source{};
                    source.sound = AnyAsset{"605c38cf-c331-43ae-8008-c1d69cea35bb"};
                    source.gain = 1.5F;
                    commands.add(ent2, source);
                    commands.add(ent2, AudioPlay{});
                }
            }
        });

    cubos.system("steal Toilet paper")
        .after(transformUpdateTag)
        .entity(0)
        .with<PlayerOwner>(0)
        .related<CollidingWith>(0, 1)
        .related<ChildOf>(2, 1)
        .entity(2)
        .with<ToiletPaper>(2)
        .related<InterpolationOf>(3, 2)
        .entity(3)
        .call([](Commands commands, Query<Entity, const PlayerOwner&, Entity, ToiletPaper&, Entity> query) {
            for (auto [stealerEnt, stealerOwner, toiletPaperEnt, toiletPaper, interpolatedEnt] : query)
            {
                if (toiletPaper.heldTime > toiletPaper.immunityTime)
                {
                    toiletPaper.player = stealerOwner.player;
                    toiletPaper.heldTime = 0.0F;
                    commands.relate(toiletPaperEnt, stealerEnt, ChildOf{});
                    commands.add(interpolatedEnt, InterpolatedDirty{});

                    AudioSource source{};
                    source.sound = AnyAsset{"c3aa1a79-a8a4-4a51-9988-4f4cef1fd68f"};
                    source.gain = 1.5F;
                    commands.add(toiletPaperEnt, source);
                    commands.add(toiletPaperEnt, AudioPlay{});
                }
            }
        });

    cubos.system("increase Toilet paper held time").call([](Query<ToiletPaper&> query, const DeltaTime& dt) {
        for (auto [toiletPaper] : query)
        {
            if (toiletPaper.player != -1)
            {
                toiletPaper.heldTime += dt.value();
            }
        }
    });

    cubos.system("give score to player holding toilet paper")
        .call([](Query<ToiletPaper&> query, const DeltaTime& dt, PlayerScores& scores) {
            scores.scoreTimer -= dt.value();
            for (auto [toiletPaper] : query)
            {
                if (toiletPaper.player != -1 && scores.scoreTimer <= 0.0F)
                {
                    scores.scores[toiletPaper.player - 1] += 1;
                }
            }
            if (scores.scoreTimer <= 0.0F)
            {
                scores.scoreTimer = 0.1F;
            }
        });
}