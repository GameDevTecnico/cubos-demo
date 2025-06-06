#include <imgui.h>

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

#include "day_night/plugin.hpp"
#include "map/plugin.hpp"
#include "map_generator/plugin.hpp"
#include "ui_effects/plugin.hpp"
#include "car/plugin.hpp"
#include "follow/plugin.hpp"
#include "interpolation/plugin.hpp"
#include "toilet_paper/plugin.hpp"
#include "player_spawner/plugin.hpp"
#include "blackout/plugin.hpp"
#include "spawn_orphan/plugin.hpp"
#include "hover_animation/plugin.hpp"
#include "round_manager/plugin.hpp"
#include "score/plugin.hpp"

using namespace cubos::engine;

static const Asset<VoxelPalette> PaletteAsset = AnyAsset("96e982ca-a18e-49e0-bf0b-2ddbbcf4d4cb");
static const Asset<Scene> IntroSceneAsset = AnyAsset("78d434eb-276b-4c80-9046-0f285bc3edfe");
static const Asset<InputBindings> DebugBindingsAsset = AnyAsset("ba1151fe-c610-499c-96c2-c67f64f24425");
static const Asset<InputBindings> Player1BindingsAsset = AnyAsset("602177be-b7e6-42b4-917e-3947c19e6c19");
static const Asset<InputBindings> Player2BindingsAsset = AnyAsset("a99968d5-ff46-48ec-a814-12d5ecdd41e9");
static const Asset<InputBindings> PlayerNBindingsAsset = AnyAsset("c32aab5e-e959-4dde-a2c9-5c9b1b68d8bc");

int main(int argc, char** argv)
{
    Cubos cubos{argc, argv};
    cubos.plugin(defaultsPlugin);
    cubos.plugin(audioPlugin);
    cubos.plugin(freeCameraPlugin);

    // Add game plugins
    cubos.plugin(roll_racers::spawnOrphanPlugin);
    cubos.plugin(roll_racers::interpolationPlugin);
    cubos.plugin(roll_racers::mapPlugin);
    cubos.plugin(roll_racers::mapGeneratorPlugin);
    cubos.plugin(roll_racers::dayNightPlugin);
    cubos.plugin(roll_racers::carPlugin);
    cubos.plugin(roll_racers::followPlugin);
    cubos.plugin(roll_racers::scorePlugin);
    cubos.plugin(roll_racers::toiletPaperPlugin);
    cubos.plugin(roll_racers::roundManagerPlugin);
    cubos.plugin(roll_racers::uiEffectsPlugin);
    cubos.plugin(roll_racers::playerSpawnerPlugin);
    cubos.plugin(roll_racers::hoverAnimationPlugin);
    cubos.plugin(roll_racers::blackoutPlugin);

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
        input.bind(*assets.read(Player2BindingsAsset), 2);
        input.bind(*assets.read(PlayerNBindingsAsset), 3);
        input.bind(*assets.read(PlayerNBindingsAsset), 4);
    });

    cubos.startupSystem("set the Voxel Palette").tagged(assetsTag).call([](Assets& assets, RenderPalette& palette) {
        palette.asset = PaletteAsset;
    });

    cubos.startupSystem("load and spawn the intro scene")
        .tagged(assetsTag)
        .call([](Commands cmds, const Assets& assets) { cmds.spawn(*assets.read(IntroSceneAsset)).named("intro"); });

    cubos.run();
}
