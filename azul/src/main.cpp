#include <imgui.h>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/scene/scene.hpp>
#include <cubos/engine/defaults/plugin.hpp>
#include <cubos/engine/render/voxels/palette.hpp>
#include <cubos/engine/render/lights/environment.hpp>
#include <cubos/engine/collisions/plugin.hpp>
#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/tools/plugin.hpp>
#include <cubos/engine/input/plugin.hpp>
#include <cubos/engine/utils/free_camera/plugin.hpp>

#include <glm/gtx/color_space.hpp>

#include "tile_map/plugin.hpp"
#include "tile_map_generator/plugin.hpp"

using namespace cubos::engine;

static const Asset<VoxelPalette> PaletteAsset = AnyAsset("/assets/voxels/main.pal");
static const Asset<Scene> MainSceneAsset = AnyAsset("/assets/scenes/main.cubos");

int main(int argc, char** argv)
{
    Cubos cubos{argc, argv};
    cubos.plugin(defaultsPlugin);
    cubos.plugin(freeCameraPlugin);
    cubos.plugin(toolsPlugin);

    // Add game plugins
    cubos.plugin(demo::tileMapPlugin);
    cubos.plugin(demo::tileMapGeneratorPlugin);

    cubos.startupSystem("configure project").before(settingsTag).call([](Settings& settings) {
        settings.setString("assets.app.osPath", APP_ASSETS_PATH);
        settings.setString("assets.builtin.osPath", BUILTIN_ASSETS_PATH);
        settings.setBool("cubos.renderer.screenPicking.enabled", false);
    });

    cubos.startupSystem("set the Voxel Palette").tagged(assetsTag).call([](Assets& assets, RenderPalette& palette) {
        palette.asset = assets.create(*assets.read(PaletteAsset));
    });

    cubos.startupSystem("set environment").call([](RenderEnvironment& environment) {
        environment.ambient = {0.4F, 0.4F, 0.4F};
        environment.skyGradient[0] = {0.6F, 1.0F, 0.8F};
        environment.skyGradient[1] = {0.25F, 0.65F, 1.0F};
    });

    cubos.startupSystem("load and spawn the Main Scene")
        .tagged(assetsTag)
        .call([](Commands cmds, const Assets& assets) { cmds.spawn(*assets.read(MainSceneAsset)).named("main"); });

    cubos.run();
}
