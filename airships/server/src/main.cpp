#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/scene/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/fixed_step/plugin.hpp>
#include <common/packets/plugin.hpp>
#include "game_server/plugin.hpp"
#include "network/plugin.hpp"
#include "welcome/plugin.hpp"

using namespace cubos::engine;

static const Asset<Scene> MainSceneAsset = AnyAsset("5c580ddf-5633-43cd-b6f6-524ad1b1cbee");

int main(int argc, char** argv)
{
    // Since the server is a barebones application, we don't need to load the window plugins, the render plugins, etc.
    // Therefore, we do not use the defaultsPlugin, and instead load only the plugins we need directly.
    Cubos cubos{argc, argv};
    cubos.plugin(settingsPlugin);
    cubos.plugin(assetsPlugin);
    cubos.plugin(scenePlugin);
    cubos.plugin(fixedStepPlugin);

    cubos.plugin(airships::common::packetsPlugin);
    cubos.plugin(airships::server::gameServerPlugin);
    cubos.plugin(airships::server::networkPlugin);

    cubos.plugin(airships::server::welcomePlugin);

    // Add game plugins
    // TODO

    cubos.startupSystem("configure Assets plugin").tagged(settingsTag).call([](Settings& settings) {
        settings.setString("assets.app.osPath", APP_ASSETS_PATH);
        settings.setString("assets.builtin.osPath", BUILTIN_ASSETS_PATH);
    });

    cubos.startupSystem("load and spawn the Main Scene")
        .tagged(assetsTag)
        .call([](Commands cmds, const Assets& assets, Settings& settings) {
            cmds.spawn(assets.read(MainSceneAsset)->blueprint);
        });

    cubos.run();
}
