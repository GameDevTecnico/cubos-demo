#include <cubos/engine/input/input.hpp>
#include <cubos/engine/input/bindings.hpp>
#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/render/voxels/palette.hpp>
#include <cubos/engine/render/lights/environment.hpp>
#include <cubos/engine/scene/scene.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/defaults/plugin.hpp>
#include <cubos/engine/tools/plugin.hpp>
#include <cubos/engine/utils/free_camera/plugin.hpp>

#include <common/empty.hpp> // TODO: here just to serve as an example

using namespace cubos::engine;

static const Asset<VoxelPalette> PaletteAsset = AnyAsset("49566681-e44e-4bec-8011-62b4ba4c5e36");
static const Asset<Scene> MainSceneAsset = AnyAsset("6dc40742-05fa-4148-a88d-89849af2e376");
static const Asset<InputBindings> Player1BindingsAsset = AnyAsset("7b7463d1-e659-4167-91c3-27267952f071");

int main(int argc, char** argv)
{
    Cubos cubos{argc, argv};
    cubos.plugin(defaultsPlugin);
    cubos.plugin(freeCameraPlugin);
    cubos.plugin(toolsPlugin);

    // Add game plugins
    // TODO

    cubos.startupSystem("configure Assets plugin").tagged(settingsTag).call([](Settings& settings) {
        settings.setString("assets.app.osPath", APP_ASSETS_PATH);
        settings.setString("assets.builtin.osPath", BUILTIN_ASSETS_PATH);
    });

    cubos.startupSystem("load and set the Input Bindings")
        .tagged(assetsTag)
        .call([](const Assets& assets, Input& input) {
            input.bind(*assets.read<InputBindings>(Player1BindingsAsset), 1);
        });

    cubos.startupSystem("set the Voxel Palette").call([](RenderPalette& palette) { palette.asset = PaletteAsset; });

    cubos.startupSystem("set environment").call([](RenderEnvironment& environment) {
        environment.ambient = {0.4F, 0.4F, 0.4F};
        environment.skyGradient[0] = {0.6F, 1.0F, 0.8F};
        environment.skyGradient[1] = {0.25F, 0.65F, 1.0F};
    });

    cubos.startupSystem("load and spawn the Main Scene")
        .tagged(assetsTag)
        .call([](Commands cmds, const Assets& assets, Settings& settings) {
            cmds.spawn(assets.read(MainSceneAsset)->blueprint);
        });

    cubos.run();
}
