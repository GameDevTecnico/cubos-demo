#include <cubos/core/settings.hpp>

#include <cubos/engine/env_settings/plugin.hpp>
#include <cubos/engine/renderer/light.hpp>
#include <cubos/engine/renderer/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/voxels/plugin.hpp>

#include "components.hpp"

using cubos::core::Settings;
using cubos::core::ecs::Commands;
using cubos::core::ecs::Query;
using cubos::core::ecs::Read;
using cubos::core::ecs::Write;
using cubos::core::gl::Grid;
using cubos::core::gl::Palette;
using namespace cubos::engine;

static const Asset<Grid> CarAsset = AnyAsset("059c16e7-a439-44c7-9bdc-6e069dba0c75");
static const Asset<Palette> PaletteAsset = AnyAsset("1aa5e234-28cb-4386-99b4-39386b0fc215");

static void settings(Write<Settings> settings)
{
    settings->setString("assets.io.path", DEMO_ASSETS_FOLDER);
}

static void setup(Commands cmds, Write<Assets> assets, Write<Renderer> renderer, Write<ActiveCamera> activeCamera)
{
    // Load the palette asset and add two colors to it.
    auto palette = assets->write(PaletteAsset);
    auto black = palette->add({{0.1F, 0.1F, 0.1F, 1.0F}});
    auto white = palette->add({{0.9F, 0.9F, 0.9F, 1.0F}});

    // Set the renderer's palette to the one we just modified.
    (*renderer)->setPalette(*palette);

    // Generate a new grid asset for the floor.
    auto floorGrid = Grid({256, 1, 256});
    for (int x = 0; x < 256; ++x)
    {
        for (int z = 0; z < 256; ++z)
        {
            floorGrid.set({x, 0, z}, (x + z) % 2 == 0 ? black : white);
        }
    }
    auto floorOffset = glm::vec3(floorGrid.size().x, 0.0F, floorGrid.size().z) / -2.0F;
    floorOffset.y = -1.0F;

    auto floorAsset = assets->create(std::move(floorGrid));

    // Spawn the floor entity.
    cmds.create(RenderableGrid{floorAsset, floorOffset}, LocalToWorld{}, Scale{4.0F});

    // Spawn the camera entity.
    activeCamera->entity =
        cmds.create(Camera{60.0F, 0.1F, 1000.0F}, LocalToWorld{})
            .add(Position{{0.0F, 120.0F, -200.0F}})
            .add(Rotation{glm::quatLookAt(glm::normalize(glm::vec3{0.0F, -1.0F, 1.0F}), glm::vec3{0.0F, 1.0F, 0.0F})})
            .entity();

    // Spawn the sun.
    cmds.create(DirectionalLight{glm::vec3(1.0F), 1.0F}, LocalToWorld{},
                Rotation{glm::quat(glm::vec3(glm::radians(45.0F), glm::radians(45.0F), 0))});
}

static void spawnCar(Commands cmds, Write<Assets> assets) {
    auto car = assets->read(CarAsset);
    glm::vec3 offset = glm::vec3(car->size().x, 0.0F, car->size().z) / -2.0F;

    cmds.create(Car{}, RenderableGrid{CarAsset, offset}, LocalToWorld{})
        .add(Position{{0.0F, 0.0F, 0.0F}})
        .add(Rotation{});
}

static void move(Query<Write<Car>, Write<Position>, Write<Rotation>> query, Read<DeltaTime> deltaTime)
{
    for (auto [entity, car, position, rotation] : query)
    {
        car->vel = rotation->quat * glm::vec3(2.0F, 0.0F, 2.0F);
        car->angVel = 0.3F;

        position->vec += deltaTime->value * car->vel;
        rotation->quat = glm::angleAxis(deltaTime->value * car->angVel, glm::vec3(0.0F, 1.0F, 0.0F)) * rotation->quat;
    }
}

int main(int argc, char** argv)
{
    Cubos cubos{argc, argv};
    cubos.addPlugin(envSettingsPlugin);
    cubos.addPlugin(rendererPlugin);
    cubos.addPlugin(voxelsPlugin);
    cubos.addComponent<Car>();

    cubos.startupSystem(settings).tagged("cubos.settings");
    cubos.startupSystem(setup).tagged("cubos.assets").afterTag("cubos.renderer.init");
    cubos.startupSystem(spawnCar).tagged("cubos.assets");
    cubos.system(move);

    cubos.run();
}
