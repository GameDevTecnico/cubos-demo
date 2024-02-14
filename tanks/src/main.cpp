#include "health/plugin.hpp"

#include <cubos/engine/prelude.hpp>
#include <cubos/engine/input/input.hpp>
#include <cubos/engine/input/bindings.hpp>
#include <cubos/engine/assets/asset.hpp>
#include <cubos/engine/assets/assets.hpp>
#include <cubos/engine/settings/settings.hpp>

#include <tesseratos/plugin.hpp>

using namespace cubos::engine;

static const Asset<InputBindings> BindingsAsset = AnyAsset("bf49ba61-5103-41bc-92e0-8a331d7842e5");

int main(int argc, char** argv)
{
    Cubos cubos{argc, argv};
    cubos.addPlugin(tesseratos::plugin);

    // Add game plugins
    cubos.addPlugin(healthPlugin);

    cubos.startupSystem("configure Assets plugin").tagged("cubos.settings").call([](Settings& settings) {
        settings.setString("assets.io.path", PROJECT_ASSETS_FOLDER);
        settings.setBool("assets.io.readOnly", false);
    });

    cubos.startupSystem("load and set the Input Bindings")
        .tagged("cubos.assets")
        .call([](const Assets& assets, Input& input) {
            auto bindings = assets.read<InputBindings>(BindingsAsset);
            input.bind(*bindings);
        });

    cubos.run();
}
