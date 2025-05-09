#include "plugin.hpp"
#include "../car/plugin.hpp"

#include <glm/glm.hpp>
#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/collisions/colliding_with.hpp>
#include <cubos/engine/physics/constraints/distance_constraint.hpp>
#include <cubos/engine/physics/plugin.hpp>
#include <cubos/engine/collisions/plugin.hpp>
#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/transform/child_of.hpp>
#include <cubos/engine/transform/local_to_world.hpp>
#include <cubos/engine/transform/position.hpp>
#include <cubos/engine/render/target/plugin.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(coffee::ToiletPaper)
{
    return cubos::core::ecs::TypeBuilder<ToiletPaper>("coffee::ToiletPaper")
        .withField("attached", &ToiletPaper::attached)
        .build();
}

CUBOS_REFLECT_IMPL(coffee::SpawnOrphan)
{
    return cubos::core::ecs::TypeBuilder<SpawnOrphan>("coffee::SpawnOrphan")
        .withField("scene", &SpawnOrphan::scene)
        .withField("entity", &SpawnOrphan::entity)
        .build();
}

void coffee::toiletPaperPlugin(Cubos& cubos)
{
    cubos.depends(transformPlugin);
    cubos.depends(physicsPlugin);
    cubos.depends(collisionsPlugin);
    cubos.depends(assetsPlugin);
    cubos.depends(renderTargetPlugin);
    cubos.depends(coffee::carPlugin);

    cubos.component<ToiletPaper>();
    cubos.component<SpawnOrphan>();

    cubos.system("spawn orphan")
        .after(transformUpdateTag)
        .after(drawToRenderTargetTag)
        .call([](Commands commands, Assets& assets,
                 Query<Entity, const SpawnOrphan&, const LocalToWorld&, Position&, const ChildOf&, Entity> query) {
            for (auto [ent1, spawnOrphan, localToWorld, pos, childOf, ent2] : query)
            {
                commands.destroy(ent1);
                commands.spawn(assets.read(spawnOrphan.scene)->blueprint())
                    .add(spawnOrphan.entity, Position{localToWorld.worldPosition()})
                    .named("orphan");
            }
        });

    cubos.system("attach to car")
        .call(
            [](Commands commands, Query<Entity, const coffee::Car&, const CollidingWith&, Entity, ToiletPaper&> query) {
                for (auto [ent1, _, collidingWith, ent2, toiletPaper] : query)
                {
                    if (!toiletPaper.attached)
                    {
                        commands.relate(ent1, ent2,
                                        DistanceConstraint{.isRigid = false,
                                                           .minDistance = 3.0F,
                                                           .maxDistance = 5.0F,
                                                           .localAnchor1 = {0.0F, 0.0F, -10.0F},
                                                           .localAnchor2 = {0.0F, 0.0F, 0.0F}});
                        toiletPaper.attached = true;
                    }
                }
            });

    cubos.system("apply gravity")
        .tagged(physicsApplyForcesTag)
        .call([](Query<Velocity&, Force&, const Mass&, const ToiletPaper&> query) {
            for (auto [velocity, force, mass, toiletPaper] : query)
            {
                // Apply gravity force
                glm::vec3 gravitationForce = mass.mass * glm::vec3(0.0F, -15.0F, 0.0F);

                force.add(gravitationForce);
            }
        });
}