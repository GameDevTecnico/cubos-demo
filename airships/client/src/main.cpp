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
#include <cubos/engine/physics/plugins/gravity.hpp>
#include <cubos/engine/render/camera/camera.hpp>

#include "islands/plugin.hpp"
#include "balloons/plugin.hpp"
#include "random_position/plugin.hpp"
#include "drivable/plugin.hpp"
#include "steering_wheel/plugin.hpp"
#include "follow/plugin.hpp"
#include "follow_controller/plugin.hpp"
#include "cannon/plugin.hpp"
#include "rudder/plugin.hpp"
#include "storm/plugin.hpp"

using namespace cubos::engine;

static const Asset<VoxelPalette> PaletteAsset = AnyAsset("0aa38b4e-df60-4b40-9c33-fd5a77e18bc2");
static const Asset<Scene> MainSceneAsset = AnyAsset("6dc40742-05fa-4148-a88d-89849af2e376");
static const Asset<InputBindings> Player1BindingsAsset = AnyAsset("7b7463d1-e659-4167-91c3-27267952f071");
static const Asset<InputBindings> DebugCameraBindingsAsset = AnyAsset("351edbb6-aeee-485c-8ede-c01c355dee7c");

int main(int argc, char** argv)
{
    Cubos cubos{argc, argv};
    cubos.plugin(defaultsPlugin);
    cubos.plugin(freeCameraPlugin);
    cubos.plugin(toolsPlugin);
    cubos.plugin(gravityPlugin);
    cubos.plugin(airships::client::followPlugin);
    cubos.plugin(airships::client::followControllerPlugin);
    cubos.plugin(airships::client::drivablePlugin);
    cubos.plugin(airships::client::steeringWheelPlugin);
    cubos.plugin(airships::client::rudderPlugin);
    cubos.plugin(airships::client::stormPlugin);
    cubos.plugin(airships::client::randomPositionPlugin);
    cubos.plugin(airships::client::islandsPlugin);
    cubos.plugin(airships::client::balloonsPlugin);
    // cubos.plugin(airships::client::cannonPlugin);

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
            input.bind(*assets.read<InputBindings>(DebugCameraBindingsAsset), 0);
        });

    cubos.startupSystem("set the Voxel Palette").call([](RenderPalette& palette) { palette.asset = PaletteAsset; });

    cubos.startupSystem("set environment").call([](RenderEnvironment& environment) {
        environment.ambient = {0.1F, 0.1F, 0.1F};
        environment.skyGradient[0] = {0.3F, 0.2F, 0.8F};
        environment.skyGradient[1] = {0.9F, 0.6F, 0.1F};
    });

    cubos.startupSystem("load and spawn the Main Scene")
        .tagged(assetsTag)
        .call([](Commands cmds, const Assets& assets, Settings& settings) {
            cmds.spawn(assets.read(MainSceneAsset)->blueprint);
        });

    cubos.startupSystem("set draw distance of every camera to something bigger")
        .tagged(assetsTag)
        .call([](Query<Camera&> query) {
            for (auto [camera] : query)
            {
                camera.zFar = 3000.0F;
            }
        });

    cubos.run();
}
