#include <imgui.h>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/scene/scene.hpp>
#include <cubos/engine/defaults/plugin.hpp>
#include <cubos/engine/render/voxels/palette.hpp>
#include <cubos/engine/render/lights/environment.hpp>
#include <cubos/engine/collisions/plugin.hpp>
#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/utils/free_camera/plugin.hpp>

#include <glm/gtx/color_space.hpp>

#include <tesseratos/plugin.hpp>

#include "follow/plugin.hpp"
#include "tile_map/plugin.hpp"
#include "tile_map_generator/plugin.hpp"
#include "walker/plugin.hpp"
#include "object/plugin.hpp"
#include "holdable/plugin.hpp"
#include "interaction/plugin.hpp"
#include "breakable/plugin.hpp"
#include "hammer/plugin.hpp"
#include "player_controller/plugin.hpp"
#include "player_spawn_point/plugin.hpp"
#include "turret/plugin.hpp"
#include "path_finding/plugin.hpp"
#include "zombie/plugin.hpp"
#include "health/plugin.hpp"
#include "progression/plugin.hpp"
#include "displays/plugin.hpp"
#include "day_night/plugin.hpp"
#include "waves/plugin.hpp"
#include "shop/plugin.hpp"
#include "skip/plugin.hpp"

using namespace cubos::engine;

static const Asset<VoxelPalette> PaletteAsset = AnyAsset("5c813e4f-8bbb-4a69-8d2a-036169deb974");
static const Asset<Scene> MainSceneAsset = AnyAsset("1d2c5bae-f6ec-4986-bc9a-d2863b317c47");

int main(int argc, char** argv)
{
    Cubos cubos{argc, argv};
    cubos.plugin(defaultsPlugin);
    cubos.plugin(freeCameraPlugin);
    cubos.plugin(tesseratos::plugin);

    // Add game plugins
    cubos.plugin(demo::followPlugin);
    cubos.plugin(demo::tileMapPlugin);
    cubos.plugin(demo::objectPlugin);
    cubos.plugin(demo::walkerPlugin);
    cubos.plugin(demo::holdablePlugin);
    cubos.plugin(demo::interactionPlugin);
    cubos.plugin(demo::playerControllerPlugin);
    cubos.plugin(demo::breakablePlugin);
    cubos.plugin(demo::hammerPlugin);
    cubos.plugin(demo::tileMapGeneratorPlugin);
    cubos.plugin(demo::healthPlugin);
    cubos.plugin(demo::pathFindingPlugin);
    cubos.plugin(demo::zombiePlugin);
    cubos.plugin(demo::turretPlugin);
    cubos.plugin(demo::progressionPlugin);
    cubos.plugin(demo::displaysPlugin);
    cubos.plugin(demo::dayNightPlugin);
    cubos.plugin(demo::wavesPlugin);
    cubos.plugin(demo::shopPlugin);
    cubos.plugin(demo::playerSpawnPointPlugin);
    cubos.plugin(demo::skipPlugin);

    cubos.startupSystem("configure Assets plugin").tagged(settingsTag).call([](Settings& settings) {
        settings.setString("assets.io.path", PROJECT_ASSETS_FOLDER);
        settings.setBool("assets.io.readOnly", false);
        settings.setBool("cubos.renderer.screenPicking.enabled", false);
    });

    cubos.startupSystem("set the Voxel Palette").tagged(assetsTag).call([](Assets& assets, RenderPalette& palette) {
        VoxelPalette modifiedPalette = *assets.read(PaletteAsset);
        for (auto& material : modifiedPalette)
        {
            auto hsv = glm::hsvColor(glm::vec3(material.color));
            hsv.y = glm::min(hsv.y * 2.0F, glm::mix(hsv.y, 1.0F, 0.75F));
            material.color = glm::vec4(glm::rgbColor(hsv), material.color.a);
        }
        palette.asset = assets.create(modifiedPalette);
    });

    cubos.startupSystem("set environment").call([](RenderEnvironment& environment) {
        environment.ambient = {0.4F, 0.4F, 0.4F};
        environment.skyGradient[0] = {0.6F, 1.0F, 0.8F};
        environment.skyGradient[1] = {0.25F, 0.65F, 1.0F};
    });

    cubos.startupSystem("load and spawn the Main Scene")
        .tagged(assetsTag)
        .call([](Commands cmds, const Assets& assets) { cmds.spawn(assets.read(MainSceneAsset)->blueprint); });

    cubos.system("reload Main Scene on game over")
        .call([](Commands cmds, const Assets& assets, Query<Entity> all, Query<const demo::PlayerController&> players,
                 demo::Progression& progression) {
            if (players.empty())
            {
                for (auto [entity] : all)
                {
                    cmds.destroy(entity);
                }

                cmds.spawn(assets.read(MainSceneAsset)->blueprint);
                progression = {};
            }
        });

    cubos.run();
}
