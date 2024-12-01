#include "plugin.hpp"

#include <cubos/core/reflection/external/glm.hpp>
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

namespace airships::client
{
    void stormPlugin(Cubos& cubos)
    {
        cubos.depends(assetsPlugin);

        cubos.startupSystem("storm").tagged(assetsTag).call([](Commands cmds, Assets& assets) {
            // TODO: spawnar storm e ir andando ao long do tempo
            auto blueprint = cmds.spawn(assets.read(StormWall)->blueprint);
            blueprint.add("player", Position{.vec = {10.0F, 5.0F, 10.0F}});
        });
    }
} // namespace airships::client
