#include "plugin.hpp"

#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/traits/enum.hpp>
#include <cubos/core/reflection/type.hpp>
#include <cubos/engine/assets/plugin.hpp>

using namespace cubos::engine;
using namespace airships::client;

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
        cubos.component<BalloonInfo>();

        cubos.startupSystem("spawn balloons").tagged(assetsTag).call([](Commands cmds, Assets& assets) {});
    }
} // namespace airships::client