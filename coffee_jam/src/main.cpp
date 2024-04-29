#include <imgui.h>

#include <cubos/engine/input/input.hpp>
#include <cubos/engine/input/bindings.hpp>
#include <cubos/engine/assets/asset.hpp>
#include <cubos/engine/assets/assets.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/scene/scene.hpp>
#include <cubos/engine/defaults/plugin.hpp>
#include <cubos/engine/renderer/plugin.hpp>
#include <cubos/engine/render/lights/environment.hpp>
#include <cubos/engine/collisions/plugin.hpp>
#include <cubos/engine/splitscreen/plugin.hpp>
#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/utils/free_camera/plugin.hpp>

#include <tesseratos/plugin.hpp>

#include "follow/plugin.hpp"
#include "tile_map/plugin.hpp"
#include "tile_map_generator/plugin.hpp"
#include "walker/plugin.hpp"
#include "object/plugin.hpp"
#include "holdable/plugin.hpp"
#include "player_controller/plugin.hpp"

using namespace cubos::engine;

static const Asset<VoxelPalette> PaletteAsset = AnyAsset("5c813e4f-8bbb-4a69-8d2a-036169deb974");
static const Asset<Scene> MainSceneAsset = AnyAsset("1d2c5bae-f6ec-4986-bc9a-d2863b317c47");
static const Asset<InputBindings> EditorBindingsAsset = AnyAsset("2f295ec8-aada-41dd-814b-281c5b5859ae");
static const Asset<InputBindings> Player1BindingsAsset = AnyAsset("602177be-b7e6-42b4-917e-3947c19e6c19");
static const Asset<InputBindings> Player2BindingsAsset = AnyAsset("ca74927e-8f5e-4e45-b9e5-184533e2a646");

int main(int argc, char** argv)
{
    Cubos cubos{argc, argv};
    cubos.plugin(defaultsPlugin);
    cubos.plugin(freeCameraPlugin);
    cubos.plugin(tesseratos::plugin);

    // Add game plugins
    cubos.plugin(demo::followPlugin);
    cubos.plugin(demo::tileMapPlugin);
    cubos.plugin(demo::tileMapGeneratorPlugin);
    cubos.plugin(demo::walkerPlugin);
    cubos.plugin(demo::objectPlugin);
    cubos.plugin(demo::holdablePlugin);
    cubos.plugin(demo::playerControllerPlugin);

    cubos.startupSystem("configure Assets plugin").tagged(settingsTag).call([](Settings& settings) {
        settings.setString("assets.io.path", PROJECT_ASSETS_FOLDER);
        settings.setBool("assets.io.readOnly", false);
    });

    cubos.startupSystem("load and set the Input Bindings")
        .tagged(assetsTag)
        .call([](const Assets& assets, Input& input) {
            input.bind(*assets.read<InputBindings>(EditorBindingsAsset), 0);
            input.bind(*assets.read<InputBindings>(Player1BindingsAsset), 1);
            input.bind(*assets.read<InputBindings>(Player2BindingsAsset), 2);
        });

    cubos.startupSystem("load and set the Voxel Palette")
        .tagged(assetsTag)
        .after(rendererInitTag)
        .call([](const Assets& assets, Renderer& renderer) {
            renderer->setPalette(*assets.read<VoxelPalette>(PaletteAsset));
        });

    cubos.startupSystem("set environment").call([](RenderEnvironment& environment) {
        environment.ambient = {0.4F, 0.4F, 0.4F};
        environment.skyGradient[0] = {0.6F, 1.0F, 0.8F};
        environment.skyGradient[1] = {0.25F, 0.65F, 1.0F};
    });

    cubos.startupSystem("load and spawn the Main Scene")
        .tagged(assetsTag)
        .call([](Commands cmds, const Assets& assets, Settings& settings, ActiveCameras& cameras) {
            if (settings.getBool("production", true))
            {
                auto builder = cmds.spawn(assets.read(MainSceneAsset)->blueprint);
                cameras.entities[0] = builder.entity("player1.base.camera");
                cameras.entities[1] = builder.entity("player2.base.camera");
            }
        });

    cubos.run();
}
