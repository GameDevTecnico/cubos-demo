#include "plugin.hpp"

#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/traits/enum.hpp>
#include <cubos/core/reflection/type.hpp>
#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/scene/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/render/voxels/grid.hpp>
#include <random>

using namespace cubos::engine;
using namespace airships::client;

CUBOS_REFLECT_EXTERNAL_IMPL(ResourceInfo::ResourceTypes)
{
    return cubos::core::reflection::Type::create("airships::client::ResourceInfo::ResourceTypes")
        .with(cubos::core::reflection::EnumTrait{}
                  .withVariant<ResourceInfo::ResourceTypes::Cannonball>("Cannonball")
                  .withVariant<ResourceInfo::ResourceTypes::Coal>("Coal")
                  .withVariant<ResourceInfo::ResourceTypes::Wood>("Wood"));
}

CUBOS_REFLECT_IMPL(ResourceInfo)
{
    return cubos::core::ecs::TypeBuilder<ResourceInfo>("airships::client::ResourceInfo")
        .withField("type", &ResourceInfo::type)
        .build();
}


namespace airships::client
{
    void resourcesPlugin(Cubos& cubos)
    {
        cubos.depends(assetsPlugin);
        cubos.depends(transformPlugin);

        cubos.component<ResourceInfo>();
    }
} // namespace airships::client