#include "plugin.hpp"
#include "../hide/plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/ecs/entity/hash.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/collisions/raycast.hpp>
#include <cubos/engine/collisions/plugin.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(airships::client::SeeThrough)
{
    return cubos::core::ecs::TypeBuilder<SeeThrough>("airships::client::SeeThrough")
        .withField("maxDistance", &SeeThrough::maxDistance)
        .build();
}

CUBOS_REFLECT_IMPL(airships::client::SeeThroughCamera)
{
    return cubos::core::ecs::TypeBuilder<SeeThroughCamera>("airships::client::SeeThroughCamera")
        .withField("mask", &SeeThroughCamera::mask)
        .build();
}

void airships::client::seeThroughPlugin(Cubos& cubos)
{
    cubos.component<SeeThrough>();
    cubos.component<SeeThroughCamera>();

    cubos.depends(transformPlugin);
    cubos.depends(collisionsPlugin);
    cubos.depends(hidePlugin);

    cubos.system("update SeeThrough objects")
        .with<Hide>()
        .with<SeeThrough>()
        .call([](Commands cmds, Raycast raycast, Query<Entity> hiddenSeeThroughQuery,
                 Query<const SeeThroughCamera&, const LocalToWorld&> cameraQuery, Query<SeeThrough&> seeThroughQuery) {
            std::unordered_set<Entity, cubos::core::ecs::EntityHash> hitEntities{};

            for (auto [camera, ltw] : cameraQuery)
            {
                Raycast::Ray ray{ltw.worldPosition(), -ltw.forward(), camera.mask};
                if (auto hit = raycast.fire(ray))
                {
                    if (auto match = seeThroughQuery.at(hit->entity))
                    {
                        auto [seeThrough] = *match;
                        if (glm::distance(ltw.worldPosition(), hit->point) < seeThrough.maxDistance)
                        {
                            hitEntities.insert(hit->entity);

                            if (!hiddenSeeThroughQuery.at(hit->entity))
                            {
                                // Only hide if not already hidden.
                                cmds.add(hit->entity, Hide{});
                            }
                        }
                    }
                }
            }

            for (auto [ent] : hiddenSeeThroughQuery)
            {
                if (!hitEntities.contains(ent))
                {
                    cmds.remove<Hide>(ent);
                }
            }
        });
}
