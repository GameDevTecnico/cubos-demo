#include <imgui.h>

#include "progression/plugin.hpp"
#include "day_night/plugin.hpp"

#include <cubos/engine/audio/plugin.hpp>
#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/scene/scene.hpp>
#include <cubos/engine/defaults/plugin.hpp>
#include <cubos/engine/render/voxels/palette.hpp>
#include <cubos/engine/render/lights/environment.hpp>
#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/input/plugin.hpp>
#include <cubos/engine/utils/free_camera/plugin.hpp>
#include <cubos/engine/render/lights/plugin.hpp>
#include <cubos/engine/tools/plugin.hpp>

#include "map/plugin.hpp"
#include "map_generator/plugin.hpp"
#include "ui_effects/plugin.hpp"
#include "car/plugin.hpp"
#include "follow/plugin.hpp"
#include "interpolation/plugin.hpp"
#include "toilet_paper/plugin.hpp"

using namespace cubos::engine;

static const Asset<VoxelPalette> PaletteAsset = AnyAsset("96e982ca-a18e-49e0-bf0b-2ddbbcf4d4cb");
static const Asset<Scene> MainSceneAsset = AnyAsset("b36dfc06-4ec3-4658-ba67-f8cb3ec787c0");
static const Asset<Scene> IntroSceneAsset = AnyAsset("78d434eb-276b-4c80-9046-0f285bc3edfe");
static const Asset<InputBindings> DebugBindingsAsset = AnyAsset("ba1151fe-c610-499c-96c2-c67f64f24425");
static const Asset<InputBindings> Player1BindingsAsset = AnyAsset("602177be-b7e6-42b4-917e-3947c19e6c19");

int main(int argc, char** argv)
{
    Cubos cubos{argc, argv};
    cubos.plugin(defaultsPlugin);
    cubos.plugin(audioPlugin);
    cubos.plugin(freeCameraPlugin);
    cubos.plugin(toolsPlugin);

    // Add game plugins
    cubos.plugin(coffee::interpolationPlugin);
    cubos.plugin(coffee::mapPlugin);
    cubos.plugin(coffee::mapGeneratorPlugin);
    cubos.plugin(coffee::progressionPlugin);
    cubos.plugin(coffee::dayNightPlugin);
    cubos.plugin(demo::followPlugin);
    cubos.plugin(coffee::uiEffectsPlugin);
    cubos.plugin(coffee::carPlugin);
    cubos.plugin(coffee::toiletPaperPlugin);

    cubos.startupSystem("set ImGui context").after(imguiInitTag).call([](ImGuiContextHolder& holder) {
        ImGui::SetCurrentContext(holder.context);
    });

    cubos.startupSystem("configure project").before(settingsTag).call([](Settings& settings) {
        settings.setString("assets.app.osPath", APP_ASSETS_PATH);
        settings.setString("assets.builtin.osPath", BUILTIN_ASSETS_PATH);
    });

    cubos.startupSystem("set the Input Bindings").tagged(assetsTag).call([](Assets& assets, Input& input) {
        input.bind(*assets.read(DebugBindingsAsset), 0);
        input.bind(*assets.read(Player1BindingsAsset), 1);
    });

    cubos.startupSystem("set the Voxel Palette").tagged(assetsTag).call([](Assets& assets, RenderPalette& palette) {
        palette.asset = PaletteAsset;
    });

    cubos.startupSystem("load and spawn the Main scene")
        .tagged(assetsTag)
        .call([](Commands cmds, const Assets& assets) { cmds.spawn(*assets.read(MainSceneAsset)).named("main"); });

    cubos.run();
}
