#include "plugin.hpp"
#include "../drivable/plugin.hpp"
#include "../damageable/plugin.hpp"
#include "../destroy_tree/plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/transform/plugin.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(airships::client::DrivableDestruction)
{
    return cubos::core::ecs::TypeBuilder<DrivableDestruction>("airships::client::DrivableDestruction")
        .withField("destroyHeight", &DrivableDestruction::destroyHeight)
        .build();
}

void airships::client::drivableDestructionPlugin(Cubos& cubos)
{
    cubos.depends(transformPlugin);
    cubos.depends(drivablePlugin);
    cubos.depends(damageablePlugin);
    cubos.depends(destroyTreePlugin);

    cubos.component<DrivableDestruction>();

    cubos.system("remove Drivable buoyancy when they no longer have health")
        .call([](Query<const DrivableDestruction&, const Damageable&, Drivable&> query) {
            for (auto [drivableDestruction, damageable, drivable] : query)
            {
                if (damageable.health <= 0)
                {
                    drivable.buoyancy = 0.0F;
                }
            }
        });

    cubos.system("destroy Drivables when they fall too low")
        .call([](Commands cmds, Query<Entity, const DrivableDestruction&, const Position&> query) {
            for (auto [entity, drivable, position] : query)
            {
                if (position.vec.y < drivable.destroyHeight)
                {
                    cmds.add(entity, DestroyTree{});
                }
            }
        });
}