#include "plugin.hpp"

#include <random>

#include <glm/glm.hpp>

#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/scene/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(coffee::MapConnector)
{
    return cubos::core::ecs::TypeBuilder<MapConnector>("coffee::MapConnector")
        .withField("position", &MapConnector::position)
        .withField("rotation", &MapConnector::rotation)
        .build();
}

CUBOS_REFLECT_IMPL(coffee::MapTile)
{
    return cubos::core::ecs::TypeBuilder<MapTile>("coffee::MapTile")
        .withField("scene", &MapTile::scene)
        .withField("connectors", &MapTile::connectors)
        .build();
}

CUBOS_REFLECT_IMPL(coffee::MapGenerator)
{
    return cubos::core::ecs::TypeBuilder<MapGenerator>("coffee::MapGenerator")
        .withField("length", &MapGenerator::length)
        .withField("start", &MapGenerator::start)
        .withField("middle", &MapGenerator::middle)
        .withField("end", &MapGenerator::end)
        .build();
}

void coffee::mapGeneratorPlugin(Cubos& cubos)
{
    cubos.depends(assetsPlugin);
    cubos.depends(scenePlugin);
    cubos.depends(transformPlugin);

    cubos.component<MapGenerator>();

    cubos.observer("generate map")
        .onAdd<MapGenerator>()
        .call([](Commands cmds, Assets& assets, Query<Entity, MapGenerator&> query) {
            for (auto [ent, generator] : query)
            {
                std::mt19937 rng(std::random_device{}());
                std::uniform_int_distribution<int> dist(0, generator.middle.size() - 1);

                CUBOS_ASSERT(generator.start.connectors.size() == 1,
                             "MapGenerator::start tile must have exactly one connector");
                auto cursor = generator.start.connectors[0];
                auto startEnt =
                    cmds.spawn(assets.read(generator.start.scene)->blueprint()).named("start").add(Position{}).entity();
                cmds.relate(startEnt, ent, ChildOf{});

                for (std::size_t i = 0; i < generator.length; ++i)
                {
                    int j = dist(rng);
                    while (true)
                    {
                        int newRot = cursor.rotation - generator.middle[j].connectors[0].rotation +
                                     generator.middle[j].connectors[1].rotation;
                        if (newRot >= -1 && newRot <= 1)
                        {
                            break;
                        }
                        // If the rotation is too high, pick another tile.
                        j = dist(rng);
                    }

                    auto& tile = generator.middle[j];
                    CUBOS_ASSERT(tile.connectors.size() == 2,
                                 "MapGenerator::middle tiles must have exactly two connectors");

                    cursor.position -= tile.connectors[0].position *
                                       glm::angleAxis(-glm::pi<float>() * 0.5F * static_cast<float>(cursor.rotation),
                                                      glm::vec3{0.0F, 1.0F, 0.0F});
                    cursor.rotation -= tile.connectors[0].rotation;
                    auto middleEnt =
                        cmds.spawn(assets.read(tile.scene)->blueprint())
                            .named("middle" + std::to_string(i))
                            .add(Position{cursor.position})
                            .add(Rotation{glm::angleAxis(glm::pi<float>() * 0.5F * static_cast<float>(cursor.rotation),
                                                         glm::vec3{0.0F, 1.0F, 0.0F})})
                            .entity();
                    cmds.relate(middleEnt, ent, ChildOf{});
                    cursor.position += tile.connectors[1].position *
                                       glm::angleAxis(-glm::pi<float>() * 0.5F * static_cast<float>(cursor.rotation),
                                                      glm::vec3{0.0F, 1.0F, 0.0F});
                    cursor.rotation += tile.connectors[1].rotation;
                }

                CUBOS_ASSERT(generator.end.connectors.size() == 1,
                             "MapGenerator::end tile must have exactly one connector");

                cursor.position -= generator.end.connectors[0].position *
                                   glm::angleAxis(-glm::pi<float>() * 0.5F * static_cast<float>(cursor.rotation),
                                                  glm::vec3{0.0F, 1.0F, 0.0F});
                cursor.rotation -= generator.end.connectors[0].rotation;
                auto endEnt =
                    cmds.spawn(assets.read(generator.end.scene)->blueprint())
                        .named("end")
                        .add(Position{cursor.position})
                        .add(Rotation{glm::angleAxis(glm::pi<float>() * 0.5F * static_cast<float>(cursor.rotation),
                                                     glm::vec3{0.0F, 1.0F, 0.0F})})
                        .entity();
                cmds.relate(endEnt, ent, ChildOf{});

                cmds.remove<MapGenerator>(ent);
            }
        });
}
