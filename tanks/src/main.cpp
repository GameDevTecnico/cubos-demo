#include "player.hpp"
#include "health/plugin.hpp"
#include "gun/plugin.hpp"
#include "bullet/plugin.hpp"
#include "particles/plugin.hpp"

#include <cubos/engine/prelude.hpp>
#include <cubos/engine/input/input.hpp>
#include <cubos/engine/input/bindings.hpp>
#include <cubos/engine/assets/asset.hpp>
#include <cubos/engine/assets/assets.hpp>
#include <cubos/engine/settings/settings.hpp>
#include <cubos/engine/physics/plugin.hpp>
#include <cubos/engine/collisions/plugin.hpp>
#include <cubos/engine/renderer/plugin.hpp>
#include <cubos/engine/renderer/environment.hpp>
#include <cubos/engine/scene/scene.hpp>

#include <tesseratos/plugin.hpp>

using namespace cubos::engine;

static const Asset<VoxelPalette> PaletteAsset = AnyAsset("1aa5e234-28cb-4386-99b4-39386b0fc215");
static const Asset<Scene> MainSceneAsset = AnyAsset("86b845ff-3dff-4eb3-b792-43f1179774a5");
static const Asset<InputBindings> EditorBindingsAsset = AnyAsset("d9bf75f2-d202-4340-b39f-33e958bdda29");
static const Asset<InputBindings> Player1BindingsAsset = AnyAsset("bf49ba61-5103-41bc-92e0-8a442d7842c3");
static const Asset<InputBindings> Player2BindingsAsset = AnyAsset("bf49ba61-5103-41bc-92e0-8a442d7842c4");

int main(int argc, char** argv)
{
    Cubos cubos{argc, argv};
    cubos.addPlugin(tesseratos::plugin);

    // Add game plugins
    cubos.addComponent<Player>();
    cubos.addPlugin(healthPlugin);
    cubos.addPlugin(collisionsPlugin);
    cubos.addPlugin(physicsPlugin);
    cubos.addPlugin(gunPlugin);
    cubos.addPlugin(bulletPlugin);
    cubos.addPlugin(explosionPlugin);

    cubos.startupSystem("configure Assets plugin").tagged("cubos.settings").call([](Settings& settings) {
        settings.setString("assets.io.path", PROJECT_ASSETS_FOLDER);
        settings.setBool("assets.io.readOnly", false);
    });

    cubos.startupSystem("load and set the Input Bindings")
        .tagged("cubos.assets")
        .call([](const Assets& assets, Input& input) {
            input.bind(*assets.read<InputBindings>(EditorBindingsAsset), 0);
            input.bind(*assets.read<InputBindings>(Player1BindingsAsset), 1);
            input.bind(*assets.read<InputBindings>(Player2BindingsAsset), 2);
        });

    cubos.startupSystem("load and set the Voxel Palette")
        .tagged("cubos.assets")
        .after("cubos.renderer.init")
        .call([](const Assets& assets, Renderer& renderer) {
            renderer->setPalette(*assets.read<VoxelPalette>(PaletteAsset));
        });

    cubos.startupSystem("set environment").call([](RendererEnvironment& environment) {
        environment.ambient = {0.4F, 0.4F, 0.4F};
        environment.skyGradient[0] = {0.6F, 1.0F, 0.8F};
        environment.skyGradient[1] = {0.25F, 0.65F, 1.0F};
    });

    cubos.startupSystem("load and spawn the Main Scene")
        .tagged("cubos.assets")
        .call([](Commands cmds, const Assets& assets, Settings& settings, ActiveCameras& cameras) {
            if (settings.getBool("production", true))
            {
                cmds.spawn(assets.read(MainSceneAsset)->blueprint);
            }
        });

    cubos.run();
}
