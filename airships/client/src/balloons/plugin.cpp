#include "plugin.hpp"
#include "../random_position/plugin.hpp"

#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/traits/enum.hpp>
#include <cubos/core/reflection/type.hpp>
#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/render/voxels/grid.hpp>
#include <cubos/engine/transform/plugin.hpp>

using namespace cubos::engine;
using namespace airships::client;

static const Asset<VoxelGrid> RedBalloonAsset = AnyAsset("b72f0154-d675-434d-989d-d789d49c9d43");

CUBOS_REFLECT_EXTERNAL_IMPL(BalloonInfo::State)
{
    return cubos::core::reflection::Type::create("airships::client::BalloonInfo::State")
        .with(cubos::core::reflection::EnumTrait{}
                  .withVariant<BalloonInfo::State::Holding>("Holding")
                  .withVariant<BalloonInfo::State::Empty>("Empty"));
}

CUBOS_REFLECT_IMPL(BalloonInfo)
{
    return cubos::core::ecs::TypeBuilder<BalloonInfo>("airships::client::BalloonInfo")
        .withField("state", &BalloonInfo::state)
        .build();
}

namespace airships::client
{
    void balloonsPlugin(Cubos& cubos)
    {
        cubos.depends(randomPositionPlugin);
        cubos.depends(assetsPlugin);
        cubos.component<BalloonInfo>();

        cubos.startupSystem("spawn balloons").tagged(assetsTag).call([](Commands cmds) {
            for (int i = 0; i < 10; i++)
            {
                cmds.create().add(RenderVoxelGrid{RedBalloonAsset}).add(LocalToWorld{}).add(RandomPosition{});
            }
        });
    }
} // namespace airships::client