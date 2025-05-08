#include "plugin.hpp"
#include "../car/plugin.hpp"

#include <glm/glm.hpp>
#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/collisions/colliding_with.hpp>
#include <cubos/engine/physics/constraints/distance_constraint.hpp>
#include <cubos/engine/physics/plugin.hpp>
#include <cubos/engine/collisions/plugin.hpp>
#include <cubos/engine/physics/solver/plugin.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(coffee::ToiletPaper)
{
    return cubos::core::ecs::TypeBuilder<ToiletPaper>("coffee::ToiletPaper")
        .withField("attached", &ToiletPaper::attached)
        .build();
}

void coffee::toiletPaperPlugin(Cubos& cubos)
{
    cubos.depends(physicsPlugin);
    cubos.depends(collisionsPlugin);
    cubos.depends(coffee::carPlugin);
    cubos.depends(physicsSolverPlugin);

    cubos.component<ToiletPaper>();

    cubos.system("attach to car")
        .call(
            [](Commands commands, Query<Entity, const coffee::Car&, const CollidingWith&, Entity, ToiletPaper&> query) {
                for (auto [ent1, _, collidingWith, ent2, toiletPaper] : query)
                {
                    if (!toiletPaper.attached)
                    {
                        CUBOS_WARN("ut");
                        commands.relate(ent1, ent2,
                                        DistanceConstraint{.entity = ent2,
                                                           .isRigid = false,
                                                           .minDistance = 0.0F,
                                                           .maxDistance = 8.0F,
                                                           .localAnchor1 = {0.0F, 0.0F, 0.0F},
                                                           .localAnchor2 = {0.0F, 0.0F, 0.0F}});
                        toiletPaper.attached = true;
                    }
                }
            });

    cubos.system("apply gravity")
        .tagged(physicsApplyForcesTag)
        .call([](Query<Entity, const Car&, const DistanceConstraint&, Entity, Force&, const Mass&, const ToiletPaper&>
                     query) {
            for (auto [ent1, car, distanceconstraint, ent2, force, mass, toiletPaper] : query)
            {
                // Apply gravity force
                glm::vec3 gravitationForce = mass.mass * glm::vec3(0.0F, -15.0F, 0.0F);

                force.add(gravitationForce);
            }
        });
}