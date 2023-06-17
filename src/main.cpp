#include <cubos/core/data/debug_serializer.hpp>
#include <cubos/core/settings.hpp>

#include <cubos/engine/env_settings/plugin.hpp>
#include <cubos/engine/renderer/light.hpp>
#include <cubos/engine/renderer/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/voxels/plugin.hpp>
#include <cubos/engine/input/plugin.hpp>

#include <vector>
#include <cmath>

#include "cameraComponent.hpp"
#include "components.hpp"

using cubos::core::Settings;
using cubos::core::ecs::Commands;
using cubos::core::data::Debug;
using cubos::core::ecs::Query;
using cubos::core::ecs::Read;
using cubos::core::ecs::Write;
using cubos::core::gl::Grid;
using cubos::core::gl::Palette;
using namespace cubos::engine;

static const Asset<Grid> CarAsset = AnyAsset("059c16e7-a439-44c7-9bdc-6e069dba0c75");
static const Asset<Grid> TrackAsset = AnyAsset("059c16e7-a439-44c7-9bdc-6e069dba0c76");
static const Asset<Palette> PaletteAsset = AnyAsset("1aa5e234-28cb-4386-99b4-39386b0fc215");
static const Asset<InputBindings> Player0BindingsAsset = AnyAsset("bf49ba61-5103-41bc-92e0-8a442d7842c3");
static const Asset<InputBindings> Player1BindingsAsset = AnyAsset("bf49ba61-5103-41bc-92e0-8a442d7842c4");

// car positions to update camera
std::vector<glm::vec3> newCarPositions(2, glm::vec3(0.0f, 0.0f, 0.0f));
std::vector<glm::quat> newCarRotations(2, glm::quat(0.0f, 0.0f, 0.0f, 0.0f));

static void settings(Write<Settings> settings)
{
    settings->setString("assets.io.path", DEMO_ASSETS_FOLDER);
}

static void loadInputBindings(Read<Assets> assets, Write<Input> input) {
    auto bindings0 = assets->read<InputBindings>(Player0BindingsAsset);
    input->bind(*bindings0, 0);
    //CUBOS_INFO("Loaded Bindings: {}", Debug(input->bindings().at(0)));

    auto bindings1 = assets->read<InputBindings>(Player1BindingsAsset);
    input->bind(*bindings1, 1);
    //CUBOS_INFO("Loaded Bindings: {}", Debug(input->bindings().at(0)));
}

static void setup(Commands cmds, Write<Assets> assets, Write<Renderer> renderer, Write<ActiveCamera> activeCamera)
{
    // Load the palette asset and add two colors to it.
    auto palette = assets->write(PaletteAsset);
    //auto black = palette->add({{0.1F, 0.1F, 0.1F, 1.0F}});
    //auto white = palette->add({{0.9F, 0.9F, 0.9F, 1.0F}});

    // Set the renderer's palette to the one we just modified.
    (*renderer)->setPalette(*palette);

    /*
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
    */
    auto trackAsset = assets->read(TrackAsset);
    auto trackOffset = glm::vec3(0.0f, 0.0f, 0.0f);
    trackOffset.y = -2.0F;

    // Spawn the floor entity.
    cmds.create(RenderableGrid{TrackAsset, trackOffset}, LocalToWorld{}, Scale{4.0F});

    // Spawn the camera entity.
    activeCamera->entity =
        cmds.create(Camera{60.0F, 0.1F, 1000.0F}, LocalToWorld{})
            .add(Position{{0.0F, 120.0F, -200.0F}})
            .add(Rotation{glm::quatLookAt(glm::normalize(glm::vec3{0.0F, -1.0F, 1.0F}), glm::vec3{0.0F, 1.0F, 0.0F})})
            .add(FollowEntity{})
            .entity();

    // Spawn the sun.
    cmds.create(DirectionalLight{glm::vec3(1.0F), 1.0F}, LocalToWorld{},
                Rotation{glm::quat(glm::vec3(glm::radians(45.0F), glm::radians(45.0F), 0))});
}

static void spawnCar(Commands cmds, Write<Assets> assets) {
    auto car = assets->read(CarAsset);
    glm::vec3 offset = glm::vec3(car->size().x, 0.0F, car->size().z) / -2.0F;

    cmds.create(Car{0,}, RenderableGrid{CarAsset, offset}, LocalToWorld{})
        .add(Position{{320.0F, 0.0F, 636.0F}})
        .add(Rotation{glm::quat(glm::vec3(0, glm::radians(90.0F), 0))});

    cmds.create(Car{1,}, RenderableGrid{CarAsset, offset}, LocalToWorld{})
        .add(Position{{320.0F, 0.0F, 667.0F}})
        .add(Rotation{glm::quat(glm::vec3(0, glm::radians(90.0F), 0))});
}

static void move(Query<Write<Car>, Write<Position>, Write<Rotation>> query, Read<Input> input, Read<DeltaTime> deltaTime)
{
    float acceleration = 35.0F;
    float drag = 2.0F;
    float maxAngVel = 250.0F;
    float angDrag = 3.0F;
    float sideDrag = 2.0F;

    float maxWheelAngle = 50.0F;
    float wheelTurnInRate = 1.0F;
    float turnSpeed = 2.0F;

    for (auto [entity, car, position, rotation] : query)
    {
        int modifier = 1.0f;
        float wheelAngle = 0.0F;
        bool handbrake = false;

        glm::vec3 forward = rotation->quat * glm::vec3(0.0f, 0.0f, 1.0f); 
        glm::vec3 side = rotation->quat * glm::vec3(1.0f, 0.0f, 0.0f);

        float absVel = glm::length(car->vel);
        float forwardVel = glm::dot(car->vel, forward);
        float sideVel = glm::dot(car->vel, side);

        if (input->pressed("space", car->id)) {
            // handbrake behavior
            handbrake = true;
            turnSpeed = 4.0F;
            sideDrag = 1.0F;
        }

        if (input->axis("vertical", car->id) != 0.0F && !handbrake) {
            CUBOS_INFO("Loaded Bindings: {}", input->axis("vertical"));
            car->vel += forward * acceleration * (input->axis("vertical", car->id)) * deltaTime->value;
        }
        else {
            if (glm::length(car->vel) < 0.05) {
                car->vel = glm::vec3(0.0F);
            }
            car->vel *= glm::max(0.0F, 1.0F - drag * deltaTime->value);
        }
        car->vel -= side * sideVel * sideDrag * deltaTime->value;

        if (input->axis("horizontal", car->id) != 0.0F) {
            wheelAngle += turnSpeed * -(input->axis("horizontal", car->id)) * deltaTime->value;
            wheelAngle = glm::clamp(wheelAngle, -maxWheelAngle, maxWheelAngle);
        }
        else {
            wheelAngle *= glm::max(0.0F, 1.0F - wheelTurnInRate * deltaTime->value);
        }
        modifier = forwardVel > 0.0f ? modifier : -modifier;
        position->vec += car->vel * deltaTime->value;
        rotation->quat = glm::angleAxis(wheelAngle * (float) sqrt(abs(forwardVel)) * modifier * 3.5f * deltaTime->value, glm::vec3(0.0F, 1.0F, 0.0F)) * rotation->quat;
    
        turnSpeed = 2.0F;
        sideDrag = 2.0F;

        newCarPositions[car->id] = position->vec;
        newCarRotations[car->id] = rotation->quat;
    }
}

static void followCar(Query<Read<Camera>, Write<Position>, Write<Rotation>, Write<FollowEntity>> query) {
    for (auto [entity, camera, position, rotation, followEntity] : query)
    {
        rotation->quat = newCarRotations[followEntity->idToFollow] * 
                         glm::angleAxis(3.1415f, glm::vec3(0.0F, 1.0F, 0.0F)) * 
                         glm::angleAxis(-0.2618f, glm::vec3(1.0f, 0.0f, 0.0f));
        position->vec = newCarPositions[followEntity->idToFollow] + 
                        (glm::vec3(0.0f, 1.0f, 0.0f) * followEntity->offset) + 
                        (glm::normalize(rotation->quat * glm::vec3(0.0f, 0.0f, 1.0f)) * 60.0f);
    }
}

int main(int argc, char** argv)
{
    Cubos cubos{argc, argv};
    cubos.addPlugin(envSettingsPlugin);
    cubos.addPlugin(rendererPlugin);
    cubos.addPlugin(voxelsPlugin);
    cubos.addPlugin(inputPlugin);
    cubos.addComponent<Car>();
    cubos.addComponent<FollowEntity>();

    cubos.startupSystem(settings).tagged("cubos.settings");
    cubos.startupSystem(loadInputBindings).tagged("cubos.assets");
    cubos.startupSystem(setup).tagged("cubos.assets").afterTag("cubos.renderer.init");
    cubos.startupSystem(spawnCar).tagged("cubos.assets");
    cubos.system(move).tagged("car.move");
    cubos.system(followCar).afterTag("car.move");

    cubos.run();
}
