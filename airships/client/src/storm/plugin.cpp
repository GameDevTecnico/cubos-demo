#include "plugin.hpp"

#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/type.hpp>
#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/transform/rotation.hpp>
#include <cubos/engine/scene/scene.hpp>

#include <random>
#include <iostream>
#include <unordered_set>
#include <memory>
#include <cstdint>
#include <map>
#include <array>

using namespace cubos::engine;

static const Asset<Scene> StormWall = AnyAsset("a4984dad-9f41-4329-9d7f-ded5d386ebf2");

CUBOS_REFLECT_IMPL(StormInfo)
{
    return cubos::core::ecs::TypeBuilder<StormInfo>("airships::client::StormInfo")
        .withField("stormRadius", &StormInfo::stormRadius)
        .build();
}

namespace airships::client
{
    void stormPlugin(Cubos& cubos)
    {
        cubos.depends(assetsPlugin);
        cubos.resource<StormInfo>();

        cubos.startupSystem("storm").tagged(assetsTag).call([](Commands cmds, Assets& assets) {

            // auto blueprint = cmds.spawn(assets.read(StormWall)->blueprint);
        });
    }
} // namespace airships::client
