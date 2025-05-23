#include <imgui.h>

#include <cubos/engine/audio/plugin.hpp>
#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/scene/scene.hpp>
#include <cubos/engine/defaults/plugin.hpp>
#include <cubos/engine/render/voxels/palette.hpp>
#include <cubos/engine/render/lights/environment.hpp>
#include <cubos/engine/collisions/plugin.hpp>
#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/input/plugin.hpp>

#include <glm/gtx/color_space.hpp>

#include "tile_map/plugin.hpp"
#include "tile_map_generator/plugin.hpp"
#include "camera_controller/plugin.hpp"
#include "player_movement/plugin.hpp"
#include "player_controller/plugin.hpp"
#include "player_spawner/plugin.hpp"
#include "waves/plugin.hpp"
#include "waves_animator/plugin.hpp"
#include "health/plugin.hpp"
#include "animation/plugin.hpp"
#include "random_grid/plugin.hpp"
#include "bullet/plugin.hpp"
#include "destroy_tree/plugin.hpp"
#include "sailor_death/plugin.hpp"
#include "coordinator/plugin.hpp"
#include "scoreboard/plugin.hpp"
#include "level/plugin.hpp"
#include "numerals/plugin.hpp"
#include "score_indicator/plugin.hpp"
#include "rotating_animation/plugin.hpp"
#include "audio/plugin.hpp"

using namespace cubos::engine;

static const Asset<VoxelPalette> PaletteAsset = AnyAsset("/assets/voxels/main.pal");

int main(int argc, char** argv)
{
    Cubos cubos{argc, argv};
    cubos.plugin(defaultsPlugin);
    cubos.plugin(audioPlugin);

    // Add game plugins
    cubos.plugin(demo::levelPlugin);
    cubos.plugin(demo::scoreboardPlugin);
    cubos.plugin(demo::destroyTreePlugin);
    cubos.plugin(demo::animationPlugin);
    cubos.plugin(demo::tileMapPlugin);
    cubos.plugin(demo::cameraControllerPlugin);
    cubos.plugin(demo::wavesPlugin);
    cubos.plugin(demo::healthPlugin);
    cubos.plugin(demo::movementPlugin);
    cubos.plugin(demo::bulletPlugin);
    cubos.plugin(demo::playerControllerPlugin);
    cubos.plugin(demo::playerSpawnerPlugin);
    cubos.plugin(demo::wavesAnimatorPlugin);
    cubos.plugin(demo::tileMapGeneratorPlugin);
    cubos.plugin(demo::randomGridPlugin);
    cubos.plugin(demo::sailorDeathPlugin);
    cubos.plugin(demo::coordinatorPlugin);
    cubos.plugin(demo::numeralsPlugin);
    cubos.plugin(demo::rotatingAnimationPlugin);
    cubos.plugin(demo::scoreIndicatorPlugin);
    cubos.plugin(demo::audioPlugin);

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

    cubos.run();
}
