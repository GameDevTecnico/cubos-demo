#include "plugin.hpp"
#include "../random_position/plugin.hpp"
#include "../destroy_tree/plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/vector.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(airships::client::LevelGenerator::Object)
{
    return cubos::core::ecs::TypeBuilder<Object>("airships::client::LevelGenerator::Object")
        .withField("scene", &Object::scene)
        .withField("amount", &Object::amount)
        .build();
}

CUBOS_REFLECT_IMPL(airships::client::LevelGenerator)
{
    return cubos::core::ecs::TypeBuilder<LevelGenerator>("airships::client::LevelGenerator")
        .withField("objects", &LevelGenerator::objects)
        .build();
}

void airships::client::levelGeneratorPlugin(Cubos& cubos)
{
    cubos.depends(assetsPlugin);
    cubos.depends(transformPlugin);
    cubos.depends(randomPositionPlugin);

    cubos.component<LevelGenerator>();

    cubos.observer("generate level")
        .onAdd<LevelGenerator>()
        .call([](Commands cmds, Assets& assets, Query<Entity, LevelGenerator&> query) {
            for (auto [ent, generator] : query)
            {
                for (auto& object : generator.objects)
                {
                    auto scene = assets.read(object.scene);
                    for (int i = 0; i < object.amount; ++i)
                    {
                        auto root = cmds.spawn(scene->blueprint).add("root", RandomPosition{}).entity("root");
                        cmds.relate(root, ent, ChildOf{});
                    }
                }
            }
        });

    cubos.observer("unload level")
        .onRemove<LevelGenerator>()
        .call([](Commands cmds, Query<Entity> query, Query<Entity, const ChildOf&> childOfQuery) {
            for (auto [ent] : query)
            {
                for (auto [child, childOf] : childOfQuery.pin(1, ent))
                {
                    cmds.add(child, DestroyTree{});
                }
            }
        });
}
