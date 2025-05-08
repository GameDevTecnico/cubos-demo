#include "plugin.hpp"
#include "../car/plugin.hpp"

#include <glm/glm.hpp>
#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/collisions/colliding_with.hpp>
#include <cubos/engine/physics/constraints/distance_constraint.hpp>
#include <cubos/engine/physics/plugin.hpp>
#include <cubos/engine/collisions/plugin.hpp>

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

    cubos.component<ToiletPaper>();

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
}