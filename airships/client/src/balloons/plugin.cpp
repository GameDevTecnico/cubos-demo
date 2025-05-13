#include "plugin.hpp"
#include "../random_position/plugin.hpp"
#include "../resource/plugin.hpp"

#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/traits/enum.hpp>
#include <cubos/core/reflection/type.hpp>
#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/scene/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/render/voxels/grid.hpp>
#include <cubos/engine/physics/components/velocity.hpp>
#include <cubos/engine/physics/plugins/gravity.hpp>
#include <cubos/engine/physics/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <random>

using namespace cubos::engine;
using namespace airships::client;

CUBOS_REFLECT_EXTERNAL_IMPL(BalloonInfo::State)
{
    return cubos::core::reflection::Type::create("airships::client::BalloonInfo::State")
        .with(cubos::core::reflection::EnumTrait{}
                  .withVariant<BalloonInfo::State::Holding>("Holding")
                  .withVariant<BalloonInfo::State::Empty>("Empty")
                  .withVariant<BalloonInfo::State::Popped>("Popped"));
}

CUBOS_REFLECT_IMPL(BalloonInfo)
{
    return cubos::core::ecs::TypeBuilder<BalloonInfo>("airships::client::BalloonInfo").wrap(&BalloonInfo::state);
}

CUBOS_REFLECT_IMPL(PopBalloon)
{
    return cubos::core::ecs::TypeBuilder<PopBalloon>("airships::client::PopBalloon").build();
}

namespace airships::client
{
    void balloonsPlugin(Cubos& cubos)
    {
        cubos.depends(physicsPlugin);
        cubos.depends(randomPositionPlugin);
        cubos.depends(assetsPlugin);
        cubos.depends(transformPlugin);
        cubos.depends(resourcesPlugin);
        cubos.depends(gravityPlugin);

        cubos.component<BalloonInfo>();
        cubos.component<PopBalloon>();

        cubos.system("idling")
            .tagged(physicsApplyForcesTag)
            .call([](Commands cmds,
                     Query<Entity, BalloonInfo&, const Position&, const RandomPosition&, const Mass&, Force&, Impulse&,
                           const Velocity&>
                         query,
                     const Gravity& gravity) {
                for (auto [ent, _, pos, rp, mass, force, imp, vel] : query)
                {
                    force.add(-mass.mass * gravity.value);
                }
            });

        cubos.system("despawn balloons").call([](Commands cmds, Query<Entity, BalloonInfo&, const Position&> query) {
            for (auto [ent, _, pos] : query)
            {
                if (pos.vec.y >= 1000.0F)
                {
                    cmds.destroy(ent);
                }
            }
        });

        cubos.system("pop balloons")
            .tagged(physicsApplyForcesTag)
            .call([](Commands cmds, Query<Entity, const ResourceInfo&, Position&, const LocalToWorld&, ChildOf&,
                                          const PopBalloon&, BalloonInfo&, Impulse&, const Velocity&, Entity>
                                        query) {
                for (auto [child, _, pos, localToWorld, childOf, pop, bi, imp, vel, parent] : query)
                {
                    bi.state = BalloonInfo::State::Popped;
                    pos.vec = localToWorld.worldPosition();
                    cmds.unrelate<ChildOf>(child, parent);
                    imp.add(glm::vec3(0.0F, 10000.0F, 0.0F));
                    cmds.remove<PopBalloon>(parent);
                }
            });
    }
} // namespace airships::client