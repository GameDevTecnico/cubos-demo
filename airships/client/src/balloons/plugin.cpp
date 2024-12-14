#include "plugin.hpp"
#include "../random_position/plugin.hpp"

#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/traits/enum.hpp>
#include <cubos/core/reflection/type.hpp>
#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/scene/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/render/voxels/grid.hpp>
#include <cubos/engine/physics/components/velocity.hpp>
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
    return cubos::core::ecs::TypeBuilder<BalloonInfo>("airships::client::BalloonInfo")
        .withField("state", &BalloonInfo::state)
        .build();
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

        cubos.component<BalloonInfo>();
        cubos.component<PopBalloon>();

        cubos.system("idling")
            .tagged(physicsApplyForcesTag)
            .call([](Commands cmds,
                     Query<Entity, BalloonInfo&, const Position&, const RandomPosition&, Impulse&, const Velocity&>
                         query) {
                for (auto [ent, _, pos, rp, imp, vel] : query)
                {
                    if (pos.vec.y <= rp.startingPos.y)
                    {
                        imp.add(glm::vec3(0.0F, 500.0F, 0.0F));
                    }
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
            .call([](Commands cmds, Query<Entity, const ResourceInfo&, Position&, const LocalToWorld&, ChildOf&, const PopBalloon&, BalloonInfo&, Impulse&, const Velocity&, Entity> query) {
                for (auto [child, _, pos, localToWorld, childOf, pop, bi, imp, vel, parent] : query)
                {
                    balloonInfo.state = BalloonInfo::State::Popped;
                    pos.vec = localToWorld.worldPosition();
                    cmds.unrelate<ChildOf>(child, parent);
                    imp.add(glm::vec3(0.0F, 8000.0F, 0.0F));
                    cmds.remove<PopBalloon>(ent);
                }
            });
    }
} // namespace airships::client