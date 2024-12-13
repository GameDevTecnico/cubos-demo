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
#include <cubos/engine/imgui/plugin.hpp>

#include "balloons/plugin.hpp"
#include "random_position/plugin.hpp"
#include "drivable/plugin.hpp"
#include "steering_wheel/plugin.hpp"
#include "follow/plugin.hpp"
#include "follow_controller/plugin.hpp"
#include "cannon/plugin.hpp"
#include "rudder/plugin.hpp"
#include "storm/plugin.hpp"
#include "player/plugin.hpp"
#include "player_id/plugin.hpp"
#include "interactable/plugin.hpp"
#include "animation/plugin.hpp"
#include "rope/plugin.hpp"
#include "hide/plugin.hpp"
#include "see_through/plugin.hpp"
#include "furnace/plugin.hpp"
#include "interpolation/plugin.hpp"
#include "auto_draws_to/plugin.hpp"
#include "auto_child_of/plugin.hpp"
#include "main_menu/plugin.hpp"
#include "level_generator/plugin.hpp"
#include "team_spawner/plugin.hpp"

using namespace cubos::engine;

static const Asset<VoxelPalette> PaletteAsset = AnyAsset("0aa38b4e-df60-4b40-9c33-fd5a77e18bc2");
static const Asset<Scene> MainSceneAsset = AnyAsset("6dc40742-05fa-4148-a88d-89849af2e376");
static const Asset<InputBindings> DebugCameraBindingsAsset = AnyAsset("351edbb6-aeee-485c-8ede-c01c355dee7c");

int main(int argc, char** argv)
{
    Cubos cubos{argc, argv};
    cubos.plugin(defaultsPlugin);
    cubos.plugin(freeCameraPlugin);
    cubos.plugin(toolsPlugin);
    cubos.plugin(gravityPlugin);

    // Add game plugins
    cubos.plugin(airships::client::interpolationPlugin);
    cubos.plugin(airships::client::animationPlugin);
    cubos.plugin(airships::client::interactablePlugin);
    cubos.plugin(airships::client::playerIdPlugin);
    cubos.plugin(airships::client::followPlugin);
    cubos.plugin(airships::client::playerPlugin);
    cubos.plugin(airships::client::followControllerPlugin);
    cubos.plugin(airships::client::drivablePlugin);
    cubos.plugin(airships::client::steeringWheelPlugin);
    cubos.plugin(airships::client::furnacePlugin);
    cubos.plugin(airships::client::rudderPlugin);
    cubos.plugin(airships::client::stormPlugin);
    cubos.plugin(airships::client::randomPositionPlugin);
    cubos.plugin(airships::client::balloonsPlugin);
    cubos.plugin(airships::client::hidePlugin);
    cubos.plugin(airships::client::ropePlugin);
    cubos.plugin(airships::client::seeThroughPlugin);
    cubos.plugin(airships::client::cannonPlugin);
    cubos.plugin(airships::client::autoDrawsToPlugin);
    cubos.plugin(airships::client::autoChildOfPlugin);
    cubos.plugin(airships::client::levelGeneratorPlugin);
    cubos.plugin(airships::client::teamSpawnerPlugin);
    cubos.plugin(airships::client::mainMenuPlugin);

    cubos.startupSystem("configure Assets plugin").tagged(settingsTag).call([](Settings& settings) {
        settings.setString("assets.app.osPath", APP_ASSETS_PATH);
        settings.setString("assets.builtin.osPath", BUILTIN_ASSETS_PATH);
    });

    cubos.startupSystem("load and set the Input Bindings")
        .tagged(assetsTag)
        .call([](const Assets& assets, Input& input) {
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

    cubos.startupSystem("set ImGui context").after(imguiInitTag).call([](ImGuiContextHolder& holder) {
        ImGui::SetCurrentContext(holder.context);
    });

    cubos.run();
}
