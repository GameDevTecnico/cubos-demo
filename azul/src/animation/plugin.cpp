#include "plugin.hpp"

#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/vector.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/assets/bridges/json.hpp>
#include <cubos/engine/render/voxels/plugin.hpp>
#include <cubos/engine/render/voxels/grid.hpp>
#include <cubos/engine/render/voxels/load.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(demo::Animation)
{
    return cubos::core::ecs::TypeBuilder<Animation>("demo::Animation")
        .withField("models", &Animation::models)
        .withField("timeBetweenFrames", &Animation::timeBetweenFrames)
        .build();
}

CUBOS_REFLECT_IMPL(demo::RenderAnimation)
{
    return cubos::core::ecs::TypeBuilder<RenderAnimation>("demo::RenderAnimation")
        .withField("animation", &RenderAnimation::animation)
        .withField("offset", &RenderAnimation::offset)
        .withField("speedMultiplier", &RenderAnimation::speedMultiplier)
        .withField("currentModel", &RenderAnimation::currentModel)
        .withField("timeSinceLastFrame", &RenderAnimation::timeSinceLastFrame)
        .build();
}

void demo::animationPlugin(Cubos& cubos)
{
    cubos.depends(assetsPlugin);
    cubos.depends(renderVoxelsPlugin);

    cubos.component<RenderAnimation>();

    cubos.observer("add RenderVoxelGrid to Animation entities")
        .onAdd<RenderAnimation>()
        .without<RenderVoxelGrid>()
        .call([](Commands cmds, Query<Entity> query) {
            for (auto [entity] : query)
            {
                cmds.add(entity, RenderVoxelGrid{});
            }
        });

    cubos.startupSystem("register Animation bridge").tagged(assetsBridgeTag).call([](Assets& assets) {
        assets.registerBridge(".anim", std::make_shared<JSONBridge<Animation>>());
    });

    cubos.system("update Animations")
        .call([](Commands cmds, const DeltaTime& dt, Assets& assets,
                 Query<Entity, RenderAnimation&, RenderVoxelGrid&> query) {
            for (auto [entity, render, grid] : query)
            {
                if (render.animation.isNull())
                {
                    continue;
                }

                render.animation = assets.load(render.animation);
                auto animation = assets.read(render.animation);

                render.timeSinceLastFrame += render.speedMultiplier * dt.value();
                if (render.timeSinceLastFrame >= animation->timeBetweenFrames)
                {
                    render.timeSinceLastFrame = 0.0F;
                    render.currentModel = (render.currentModel + 1) % animation->models.size();

                    // Make the asset stay loaded by storing a strong reference to it here
                    render.currentModels.resize(animation->models.size());
                    render.currentModels[render.currentModel] = assets.load(animation->models[render.currentModel]);
                    grid.asset = render.currentModels[render.currentModel];
                    grid.offset = render.offset;

                    // Add a LoadRenderVoxels to switch the mesh
                    cmds.add(entity, LoadRenderVoxels{});
                }
            }
        });
}
