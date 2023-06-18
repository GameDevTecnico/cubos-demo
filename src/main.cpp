#include <cubos/core/data/debug_serializer.hpp>
#include <cubos/core/settings.hpp>

#include <cubos/engine/env_settings/plugin.hpp>
#include <cubos/engine/renderer/environment.hpp>
#include <cubos/engine/renderer/light.hpp>
#include <cubos/engine/renderer/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/voxels/plugin.hpp>
#include <cubos/engine/input/plugin.hpp>
#include <cubos/engine/scene/plugin.hpp>

#include <vector>
#include <cmath>

//  aahhhhhh

#include <cubos/core/geom/box.hpp>
#include <cubos/core/gl/debug.hpp>
#include <cubos/core/log.hpp>

#include <cubos/engine/collisions/aabb.hpp>
#include <cubos/engine/collisions/broad_phase_collisions.hpp>
#include <cubos/engine/collisions/colliders/box.hpp>
#include <cubos/engine/collisions/plugin.hpp>

// ------------

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
static const Asset<Scene> SceneAsset = AnyAsset("059c16e7-a439-44c7-9bdc-6e069dba0c77");
static const Asset<InputBindings> Player0BindingsAsset = AnyAsset("bf49ba61-5103-41bc-92e0-8a442d7842c3");
static const Asset<InputBindings> Player1BindingsAsset = AnyAsset("bf49ba61-5103-41bc-92e0-8a442d7842c4");

// car positions to update camera
std::vector<glm::vec3> newCarPositions(2, glm::vec3(0.0f, 0.0f, 0.0f));
std::vector<glm::quat> newCarRotations(2, glm::quat(0.0f, 0.0f, 0.0f, 0.0f));


struct DayLights
{
    int ambientLight;
    int directionalLight;
};

/*
struct NightLights
{
    int headLight1;
    int headLight2;
};
*/

struct IsDay {
    bool value = true;
    bool switching = false;
};


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

static void setup(Commands cmds, Write<Assets> assets, Write<Renderer> renderer, Write<ActiveCameras> activeCameras, Write<RendererEnvironment> env)
{
    // Load the palette asset and add two colors to it.
    auto palette = assets->write(PaletteAsset);
    for(int i = 1; i <= palette->size(); i++) {
        CUBOS_INFO("Palette: {}, {}", i, Debug(palette->get(i)));
    }
    palette->set(6, {{10.0F, 0.1F, 0.1F, 1.0F}});

    auto scene = assets->read(SceneAsset);
    cmds.spawn(scene->blueprint);
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
    //cmds.create(RenderableGrid{TrackAsset, trackOffset}, LocalToWorld{}, Scale{4.0F});

    // Spawn the camera entity.
    activeCameras->entities[0] =
        cmds.create(Camera{60.0F, 0.1F, 1000.0F}, LocalToWorld{})
            .add(Position{{0.0F, 120.0F, -200.0F}})
            .add(Rotation{glm::quatLookAt(glm::normalize(glm::vec3{0.0F, -1.0F, 1.0F}), glm::vec3{0.0F, 1.0F, 0.0F})})
            .add(FollowEntity{0,})
            .entity();

    // Spawn the camera entity.
    activeCameras->entities[1] =
        cmds.create(Camera{60.0F, 0.1F, 1000.0F}, LocalToWorld{})
            .add(Position{{0.0F, 120.0F, -200.0F}})
            .add(Rotation{glm::quatLookAt(glm::normalize(glm::vec3{0.0F, -1.0F, 1.0F}), glm::vec3{0.0F, 1.0F, 0.0F})})
            .add(FollowEntity{1,})
            .entity();

    // Spawn the sun.
    cmds.create(DirectionalLight{glm::vec3(1.0F), 1.0F}, LocalToWorld{},
                Rotation{glm::quat(glm::vec3(glm::radians(45.0F), glm::radians(45.0F), 0))});
    
    /*
    cmds.create(SpotLight{.color = {1.0F, 1.0F, 1.0F}, .intensity = 3.0F, .range = 200.0F, .spotAngle = 70.0F, .innerSpotAngle = 60.0F}, LocalToWorld{})
        .add(Position{{360.0F, 10.0F, 620.0F}})
        .add(Rotation{glm::quat(glm::vec3(glm::radians(-90.0F), 0, glm::radians(-90.0F)))});
    */

    // Set the ambient light.
    //env->ambient = {0.5F, 0.5F, 0.5F};
    //env->skyGradient[0] = {0.6F, 1.0F, 0.8F};
    //env->skyGradient[1] = {0.25F, 0.65F, 1.0F};
    //env->skyGradient[0] = {0.8F, 0.8F, 1.0F};
    //env->skyGradient[1] = {0.3F, 0.0F, 0.8F};
}

static void spawnCar(Commands cmds, Write<Assets> assets, Write<BroadPhaseCollisions> collisions) {
    auto car = assets->read(CarAsset);
    glm::vec3 offset = glm::vec3(car->size().x, 0.0F, car->size().z) / -2.0F;

    auto entity1 = cmds.create(Car{0,}, RenderableGrid{CarAsset, offset}, LocalToWorld{})
        .add(Position{{320.0F, 0.0F, 636.0F}})
        .add(Rotation{glm::quat(glm::vec3(0, glm::radians(90.0F), 0))})
        .add(BoxCollider{.shape = {{6.0, 6.0, 14.0}}})
        .entity();

    collisions->addEntity(entity1);

    auto entity2 = cmds.create(Car{1,}, RenderableGrid{CarAsset, offset}, LocalToWorld{})
        .add(Position{{320.0F, 0.0F, 667.0F}})
        .add(Rotation{glm::quat(glm::vec3(0, glm::radians(90.0F), 0))})
        .add(BoxCollider{.shape = {{6.0, 6.0, 14.0}}})
        .entity();
        
    collisions->addEntity(entity2);

    cmds.create(SpotLight{.color = {1.0F, 1.0F, 1.0F}, .intensity = 4.0F, .range = 200.0F, .spotAngle = 70.0F, .innerSpotAngle = 60.0F}, LocalToWorld{})
        .add(Position{{340.0F, 10.0F, 636.0F}})
        .add(Rotation{glm::quat(glm::vec3(glm::radians(-90.0F), 0, glm::radians(-90.0F)))})
        .add(FollowEntity{.idToFollow = 0, .offset = {0.0f, 10.0f, 0.0f}});

    cmds.create(SpotLight{.color = {1.0F, 1.0F, 1.0F}, .intensity = 4.0F, .range = 200.0F, .spotAngle = 70.0F, .innerSpotAngle = 60.0F}, LocalToWorld{})
        .add(Position{{340.0F, 10.0F, 667.0}})
        .add(Rotation{glm::quat(glm::vec3(glm::radians(-90.0F), 0, glm::radians(-90.0F)))})
        .add(FollowEntity{.idToFollow = 1, .offset = {0.0f, 10.0f, 0.0f}});
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

static void switchDayNight(Write<IsDay> isDay, Read<Input> input) {
    if (input->pressed("space") && !isDay->switching) {
        isDay->value = !isDay->value;
        isDay->switching = true;
    }
    else if (!input->pressed("space")) {
        isDay->switching = false;
    }
}

static void handleDayLights(Query<Write<DirectionalLight>> query, Read<IsDay> isDay, Write<RendererEnvironment> env) {
    if (isDay->value) {
        for (auto [entity, light] : query)
        { 
            light->intensity = 0.7f;
        }
        env->ambient = {0.5F, 0.5F, 0.5F};
        env->skyGradient[0] = {0.6F, 1.0F, 0.8F};
        env->skyGradient[1] = {0.25F, 0.65F, 1.0F};
    }
    else {
        for (auto [entity, light] : query)
        { 
            light->intensity = 0.0f;
        }
        env->ambient = {0.1F, 0.1F, 0.1F};
        env->skyGradient[0] = {0.8F, 0.8F, 1.0F};
        env->skyGradient[1] = {0.3F, 0.0F, 0.8F};
    }
}

static void handleNightLights(Query<Write<SpotLight>, Write<Position>, Write<Rotation>, Write<FollowEntity>> query, Read<IsDay> isDay) { //  Read<Input> input
    for (auto [entity, light, position, rotation, followEntity] : query)
    {
        if (!isDay->value) {
            light->intensity = 4.0f;
        }
        else {
            light->intensity = 0.0f;
        }

        rotation->quat = newCarRotations[followEntity->idToFollow] * glm::quat(glm::vec3(0, 0, glm::radians(-90.0F))); // glm::angleAxis(-0.2618f, glm::vec3(1.0f, 0.0f, 0.0f));
                         //glm::angleAxis(3.1415f, glm::vec3(0.0F, 1.0F, 0.0F)) * 
                         //glm::angleAxis(-0.2618f, glm::vec3(1.0f, 0.0f, 0.0f));
        position->vec = newCarPositions[followEntity->idToFollow] + 
                        (glm::vec3(0.0f, 1.0f, 0.0f) * followEntity->offset) + 
                        (glm::normalize(rotation->quat * glm::vec3(0.0f, 0.0f, 1.0f)) * 20.0f);
    }
}


// -------------------------------------------------------------------------------------
//                          I have no idea what I'm doing zone
// -------------------------------------------------------------------------------------

/*
static void addColliders(Write<State> state, Write<BroadPhaseCollisions> collisions, Commands commands)
{
    state->a = commands.create()
                   .add(BoxCollider{})
                   .add(LocalToWorld{})
                   .add(Position{glm::vec3{0.0F, 0.0F, -2.0F}})
                   .add(Rotation{})
                   .entity();
    state->aRotationAxis = glm::sphericalRand(1.0F);
    collisions->addEntity(state->a);

    state->b = commands.create()
                   .add(BoxCollider{})
                   .add(LocalToWorld{})
                   .add(Position{glm::vec3{0.0F, 0.0F, 2.0F}})
                   .add(Rotation{})
                   .entity();
    state->bRotationAxis = glm::sphericalRand(1.0F);
    collisions->addEntity(state->b);
}

static void updateTransform(Write<State> state, Read<Input> input, Query<Write<Position>, Write<Rotation>> query)
{
    auto [aPos, aRot] = query[state->a].value();
    auto [bPos, bRot] = query[state->b].value();

    if (state->collided)
    {
        if (input->pressed("reset"))
        {
            state->collided = false;

            aPos->vec = glm::vec3{0.0F, 0.0F, -2.0F};
            aRot->quat = glm::quat{1.0F, 0.0F, 0.0F, 0.0F};
            state->aRotationAxis = glm::sphericalRand(1.0F);

            bPos->vec = glm::vec3{0.0F, 0.0F, 2.0F};
            bRot->quat = glm::quat{1.0F, 0.0F, 0.0F, 0.0F};
            state->bRotationAxis = glm::sphericalRand(1.0F);
        }
        return;
    }

    aRot->quat = glm::rotate(aRot->quat, 0.01F, state->aRotationAxis);
    aPos->vec += glm::vec3{0.0F, 0.0F, 0.01F};

    bRot->quat = glm::rotate(bRot->quat, 0.01F, state->bRotationAxis);
    bPos->vec += glm::vec3{0.0F, 0.0F, -0.01F};
}
*/

static void carCollision(Query<Read<BoxCollider>, Read<Car>, Read<LocalToWorld>> query, Read<BroadPhaseCollisions> collisions)
{
    for (auto [entity, collider, car, transform] : query)
    {
        for (auto& entities : collisions->candidates(BroadPhaseCollisions::CollisionType::BoxBox))
        {
            BoxCollider otherCollider;
            LocalToWorld otherTransform;
            if (entities.first == entity)
            {
                auto [collider2, car2, transform2] = query[entities.second].value();
                otherCollider = *collider2;
                otherTransform = *transform2;
            }
            else if (entities.second == entity)
            {
                auto [collider1, car1, transform1] = query[entities.first].value();
                otherCollider = *collider1;
                otherTransform = *transform1;
            }
            else {
                break;
            }
            
            CUBOS_INFO("HIT!!!!!!!!!");
            return;
        }
    }
}


static void debugRender(Query<Read<LocalToWorld>, Read<BoxCollider>, Read<ColliderAABB>> query)
{
    for (auto [entity, localToWorld, collider, aabb] : query)
    {
        cubos::core::gl::Debug::drawWireBox(collider->shape, localToWorld->mat * collider->transform);
        cubos::core::gl::Debug::drawWireBox(aabb->box(), glm::translate(glm::mat4{1.0}, aabb->center()),
                                            glm::vec3{1.0, 0.0, 0.0});
    }
}





int main(int argc, char** argv)
{
    Cubos cubos{argc, argv};
    cubos.addPlugin(envSettingsPlugin);
    cubos.addPlugin(rendererPlugin);
    cubos.addPlugin(voxelsPlugin);
    cubos.addPlugin(inputPlugin);
    cubos.addPlugin(scenePlugin);
    cubos.addComponent<Car>();
    cubos.addComponent<FollowEntity>();
    cubos.addResource<IsDay>();

    cubos.addPlugin(collisionsPlugin);
    cubos.addPlugin(rendererPlugin);
    cubos.addPlugin(inputPlugin);

    cubos.startupSystem(settings).tagged("cubos.settings");
    cubos.startupSystem(loadInputBindings).tagged("cubos.assets");
    cubos.startupSystem(setup).tagged("cubos.assets").afterTag("cubos.renderer.init");
    cubos.startupSystem(spawnCar).tagged("cubos.assets");
    cubos.system(debugRender);
    cubos.system(carCollision).afterTag("cubos.collisions");
    cubos.system(move).tagged("car.move").beforeTag("cubos.transform.update");
    cubos.system(followCar).afterTag("car.move");
    cubos.system(switchDayNight);
    cubos.system(handleNightLights);
    cubos.system(handleDayLights);

    cubos.run();
}
