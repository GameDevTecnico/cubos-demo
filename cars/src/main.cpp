#include "car/plugin.hpp"
#include "car/car.hpp"
#include "day_night/plugin.hpp"
#include "explosion/plugin.hpp"
#include "race/plugin.hpp"
#include "race/racer.hpp"
#include "dead/plugin.hpp"

#include <imgui.h>

#include <cubos/engine/input/input.hpp>
#include <cubos/engine/input/bindings.hpp>
#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/scene/scene.hpp>
#include <cubos/engine/defaults/plugin.hpp>
#include <cubos/engine/render/lights/environment.hpp>
#include <cubos/engine/render/voxels/palette.hpp>
#include <cubos/engine/collisions/plugin.hpp>
#include <cubos/engine/splitscreen/plugin.hpp>
#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/utils/free_camera/plugin.hpp>

#include <tesseratos/plugin.hpp>

using namespace cubos::engine;

static const Asset<VoxelPalette> PaletteAsset = AnyAsset("1aa5e234-28cb-4386-99b4-39386b0fc215");
static const Asset<Scene> MainSceneAsset = AnyAsset("059c16e7-a439-44c7-9bdc-6e069dba0c77");
static const Asset<InputBindings> EditorBindingsAsset = AnyAsset("d9bf75f2-d202-4340-b39f-33e958bdda29");
static const Asset<InputBindings> Player1BindingsAsset = AnyAsset("bf49ba61-5103-41bc-92e0-8a442d7842c3");
static const Asset<InputBindings> Player2BindingsAsset = AnyAsset("bf49ba61-5103-41bc-92e0-8a442d7842c4");

int main(int argc, char** argv)
{
    Cubos cubos{argc, argv};
    cubos.plugin(defaultsPlugin);
    cubos.plugin(freeCameraPlugin);
    cubos.plugin(tesseratos::plugin);

    // Add game components and plugins.
    cubos.plugin(demo::deadPlugin);
    cubos.plugin(demo::carPlugin);
    cubos.plugin(demo::dayNightPlugin);
    cubos.plugin(demo::explosionPlugin);
    cubos.plugin(demo::racePlugin);

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

    cubos.startupSystem("set the Voxel Palette").call([](RenderPalette& palette) { palette.asset = PaletteAsset; });

    cubos.startupSystem("set environment").call([](RenderEnvironment& environment) {
        environment.ambient = {0.4F, 0.4F, 0.4F};
        environment.skyGradient[0] = {0.6F, 1.0F, 0.8F};
        environment.skyGradient[1] = {0.25F, 0.65F, 1.0F};
    });

    cubos.startupSystem("load and spawn the Main Scene")
        .tagged(assetsTag)
        .call([](Commands cmds, const Assets& assets, Settings& settings) {
            if (settings.getBool("production", true))
            {
                cmds.spawn(assets.read(MainSceneAsset)->blueprint);
            }
        });

    cubos.system("show lap times").tagged(imguiTag).call([](Query<const demo::Car&, const demo::Racer&> query) {
        for (auto [car, racer] : query)
        {
            std::string title = "Lap Times for Player " + std::to_string(car.player);
            ImGui::Begin(title.c_str());

            float bestLapTime = INFINITY;
            for (auto lapTime : racer.lapTimes)
            {
                bestLapTime = glm::min(bestLapTime, lapTime);
            }

            ImGui::Text("Current Lap Time: %f", racer.currentLapTime);
            ImGui::Text("Current Successive Laps: %d", racer.successiveLaps);
            ImGui::Text("Best Lap Time: %f", bestLapTime);
            ImGui::Text("Best Successive Laps: %d", racer.maxSuccessiveLaps);
            ImGui::Separator();
            for (int i = 0; i < racer.lapTimes.size(); i++)
            {
                ImGui::Text("Lap %d Time: %f", i + 1, racer.lapTimes[i]);
            }

            ImGui::End();
        }
    });

    cubos.run();
}
