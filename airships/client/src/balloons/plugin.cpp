#include "plugin.hpp"
#include "../random_position/plugin.hpp"

#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/traits/enum.hpp>
#include <cubos/core/reflection/type.hpp>
#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/scene/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>

using namespace cubos::engine;
using namespace airships::client;

static const Asset<Scene> RedBalloonAsset = AnyAsset("8816c9f2-cecd-40f5-bf0d-80b6d5078ed1");

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

        cubos.startupSystem("spawn balloons").tagged(assetsTag).call([](Commands cmds, Assets& assets) {
            for (int i = 0; i < 30; i++)
            {
                cmds.spawn(assets.read(RedBalloonAsset)->blueprint).add("root", RandomPosition{});
            }
        });
    }
} // namespace airships::client