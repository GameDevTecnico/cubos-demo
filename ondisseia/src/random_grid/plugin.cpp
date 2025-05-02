#include "plugin.hpp"

#include <cubos/core/reflection/external/vector.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/render/voxels/plugin.hpp>
#include <cubos/engine/render/voxels/grid.hpp>

#include <random>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(demo::RandomGrid)
{
    return cubos::core::ecs::TypeBuilder<RandomGrid>("demo::RandomGrid")
        .withField("models", &RandomGrid::models)
        .withField("offset", &RandomGrid::offset)
        .build();
}

void demo::randomGridPlugin(Cubos& cubos)
{
    cubos.depends(assetsPlugin);
    cubos.depends(renderVoxelsPlugin);

    cubos.component<RandomGrid>();

    cubos.observer("add RenderVoxelGrid to RandomGrid entities")
        .onAdd<RandomGrid>()
        .call([](Commands cmds, Query<Entity, const RandomGrid&> query) {
            for (auto [entity, random] : query)
            {
                int index = rand() % random.models.size();
                cmds.add(entity, RenderVoxelGrid{
                                     .asset = random.models[index],
                                     .offset = random.offset,
                                 });
                cmds.remove<RandomGrid>(entity);
            }
        });
}
