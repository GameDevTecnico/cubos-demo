#include <cubos/engine/cubos.hpp>
#include <cubos/core/ecs/system/query.hpp>

#include <cubos/engine/renderer/environment.hpp>
#include <cubos/engine/renderer/directional_light.hpp>
#include <cubos/engine/renderer/plugin.hpp>
#include <cubos/engine/settings/settings.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/voxels/plugin.hpp>
#include <cubos/engine/input/plugin.hpp>
#include <cubos/engine/scene/plugin.hpp>
#include <cubos/engine/physics/plugin.hpp>

#include <vector>
#include <cmath>
#include <imgui.h>

#include "player/plugin.hpp"

using cubos::core::ecs::Commands;
using cubos::core::ecs::Query;
using cubos::core::ecs::Read;
using cubos::core::ecs::Write;

using namespace cubos::engine;

static const Asset<VoxelGrid> PlatformAsset = AnyAsset("059c16e7-a439-44c7-9bdc-6e069dba0c76");
static const Asset<VoxelPalette> PaletteAsset = AnyAsset("1aa5e234-28cb-4386-99b4-39386b0fc215");
static const Asset<Scene> SceneAsset = AnyAsset("059c16e7-a439-44c7-9bdc-6e069dba0c77");

static void settings(Write<Settings> settings)
{
    settings->setString("assets.io.path", PROJECT_ASSETS_FOLDER);
}

static void setup(Commands cmds, Write<Assets> assets, Write<Renderer> renderer, Write<ActiveCameras> activeCameras,
                  Write<RendererEnvironment> env, Write<Damping> damping, Write<Gravity> gravity)
{
    // Load the palette asset and add two colors to it.
    auto palette = assets->write(PaletteAsset);

    // Set the renderer's palette to the one we just modified.
    (*renderer)->setPalette(*palette);

    // Spawn the sun.
    cmds.create()
        .add(DirectionalLight{glm::vec3(1.0F), 0.7F})
        .add(Rotation{glm::quat(glm::vec3(glm::radians(45.0F), glm::radians(45.0F), 0))});

    env->ambient = {0.4F, 0.4F, 0.4F};
    env->skyGradient[0] = {0.6F, 1.0F, 0.8F};
    env->skyGradient[1] = {0.25F, 0.65F, 1.0F};

    damping->value = 0.99F;
    gravity->value = glm::vec3{0.0F, -25.0F, 0.0F};
}

int main(int argc, char** argv)
{
    Cubos cubos{argc, argv};

    cubos.addPlugin(rendererPlugin);
    cubos.addPlugin(voxelsPlugin);
    cubos.addPlugin(inputPlugin);
    cubos.addPlugin(scenePlugin);
    cubos.addPlugin(physicsPlugin);

    cubos.addPlugin(demo::playersPlugin);

    cubos.startupSystem(settings).tagged("cubos.settings");
    cubos.startupSystem(setup).tagged("cubos.assets").after("cubos.renderer.init");

    cubos.run();
}
