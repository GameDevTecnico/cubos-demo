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

static const Asset<Scene> BalloonSceneAsset = AnyAsset("8816c9f2-cecd-40f5-bf0d-80b6d5078ed1");

static const Asset<VoxelGrid> RedBalloonAsset = AnyAsset("b72f0154-d675-434d-989d-d789d49c9d43");
static const Asset<VoxelGrid> PurpleBalloonAsset = AnyAsset("aa6b6b2e-cce6-428d-8494-e3e08adcc319");
static const Asset<VoxelGrid> YellowBalloonAsset = AnyAsset("401335af-480d-4cca-a50a-003041525333");

static const std::array<Asset<VoxelGrid>, 3> balloons = {RedBalloonAsset, PurpleBalloonAsset, YellowBalloonAsset};

static const Asset<VoxelGrid> CannonBallAsset = AnyAsset("cd9e1c30-0a1b-4f88-b7f9-c4f7b95f2b63");

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

        cubos.startupSystem("spawn balloons").tagged(assetsTag).call([](Commands cmds, Assets& assets) {
            std::mt19937 engine{std::random_device()()};
            std::uniform_int_distribution balloonType(0, static_cast<int>(balloons.size() - 1));
            for (int i = 0; i < 15; i++)
            {
                int balloontype = balloonType(engine);

                cmds.spawn(assets.read(BalloonSceneAsset)->blueprint)
                    .add("root", BalloonInfo{})
                    .add("root", RandomPosition{})
                    .add("root", RenderVoxelGrid{balloons[balloontype], {-4.5F, -14.0F, -4.5F}})
                    .add("resource", RenderVoxelGrid{CannonBallAsset, {-3.5F, -3.5F, -3.5F}});
            }
        });

        cubos.system("idling")
            .tagged(physicsApplyForcesTag)
            .call([](Commands cmds, Query<Entity, BalloonInfo&, const Position&, const RandomPosition&, Impulse&, const Velocity&> query) {
                for (auto [ent, _, pos, rp, imp, vel] : query)
                {
                    if (pos.vec.y <= rp.startingPos.y)
                    {
                        imp.add(glm::vec3(0.0F, 500.0F, 0.0F));
                    }
                }
            });

        cubos.system("despawn balloons")
            .call([](Commands cmds, Query<Entity, BalloonInfo&, const Position&> query) {
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
            .call([](Commands cmds, Query<Entity, const PopBalloon&, BalloonInfo&, Impulse&, const Velocity&> query) {
                for (auto [ent, _, balloonInfo, imp, vel] : query)
                {
                    balloonInfo.state = BalloonInfo::State::Popped;
                    imp.add(glm::vec3(0.0F, 10000.0F, 0.0F));
                    cmds.remove<PopBalloon>(ent);
                }
            });
    }
} // namespace airships::client