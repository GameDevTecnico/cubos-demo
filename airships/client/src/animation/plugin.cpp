#include "plugin.hpp"

#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/vector.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/render/voxels/plugin.hpp>
#include <cubos/engine/render/voxels/grid.hpp>
#include <cubos/engine/render/voxels/load.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(airships::client::Animation)
{
    return cubos::core::ecs::TypeBuilder<Animation>("airships::client::Animation")
        .withField("models", &Animation::models)
        .withField("offset", &Animation::offset)
        .withField("timeBetweenFrames", &Animation::timeBetweenFrames)
        .withField("currentModel", &Animation::currentModel)
        .withField("timeSinceLastFrame", &Animation::timeSinceLastFrame)
        .build();
}

void airships::client::animationPlugin(Cubos& cubos)
{
    cubos.depends(assetsPlugin);
    cubos.depends(renderVoxelsPlugin);

    cubos.component<Animation>();

    cubos.observer("add RenderVoxelGrid to Animation entities")
        .onAdd<Animation>()
        .without<RenderVoxelGrid>()
        .call([](Commands cmds, Query<Entity> query) {
            for (auto [entity] : query)
            {
                cmds.add(entity, RenderVoxelGrid{});
            }
        });

    cubos.system("update Animations")
        .call([](Commands cmds, const DeltaTime& dt, Assets& assets,
                 Query<Entity, Animation&, RenderVoxelGrid&> query) {
            for (auto [entity, animation, grid] : query)
            {
                animation.timeSinceLastFrame += dt.value();
                if (animation.timeSinceLastFrame >= animation.timeBetweenFrames)
                {
                    animation.timeSinceLastFrame = 0.0F;
                    animation.currentModel = (animation.currentModel + 1) % animation.models.size();

                    // Make the asset stay loaded by storing a strong reference to it here
                    animation.models[animation.currentModel] = assets.load(animation.models[animation.currentModel]);
                    grid.asset = animation.models[animation.currentModel];
                    grid.offset = animation.offset;

                    // Add a LoadRenderVoxels to switch the mesh
                    cmds.add(entity, LoadRenderVoxels{});
                }
            }
        });
}
