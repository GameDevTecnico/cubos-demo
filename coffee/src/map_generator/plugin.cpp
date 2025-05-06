#include "plugin.hpp"
#include "../map/plugin.hpp"

#include <random>

#include <glm/glm.hpp>

#include <cubos/core/ecs/reflection.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(coffee::MapGenerator)
{
    return cubos::core::ecs::TypeBuilder<MapGenerator>("coffee::MapGenerator")
        .withField("tileSize", &MapGenerator::tileSize)
        .withField("trackLength", &MapGenerator::trackLength)
        .withField("startTileScene", &MapGenerator::startTileScene)
        .withField("endTileScene", &MapGenerator::endTileScene)
        .withField("straightTileScene", &MapGenerator::straightTileScene)
        .withField("curveLeftTileScene", &MapGenerator::curveLeftTileScene)
        .withField("curveRightTileScene", &MapGenerator::curveRightTileScene)
        .build();
}

static glm::ivec2 rotationToDirection(int rotation)
{
    rotation = (rotation + 4) % 4;
    switch (rotation)
    {
    case 0:
        return {0, 1};
    case 1:
        return {-1, 0};
    case 2:
        return {0, -1};
    case 3:
        return {1, 0};
    default:
        CUBOS_FAIL("Rotation must be between 0 and 3");
    }
}

void coffee::mapGeneratorPlugin(Cubos& cubos)
{
    cubos.depends(mapPlugin);

    cubos.component<MapGenerator>();

    cubos.observer("generate Map")
        .onAdd<MapGenerator>()
        .call([](Commands cmds, Query<Entity, MapGenerator&> query) {
            std::mt19937 rng(std::random_device{}());
            std::uniform_real_distribution<float> chanceDist(0.0F, 1.0F);

            for (auto [ent, generator] : query)
            {
                Map map{};
                map.tileSize = generator.tileSize;

                // Start by placing the main road.
                map.tiles[{0, 0}] = {generator.startTileScene, 0};
                glm::ivec2 cursorPosition{0, 1};
                int cursorRotation = 0;

                for (std::size_t i = 0; i < generator.trackLength; ++i)
                {
                    if (chanceDist(rng) < 0.6F)
                    {
                        bool left = chanceDist(rng) < 0.5F;
                        if (cursorRotation == -1 && left || cursorRotation == 1 && !left)
                        {
                            // If the rotation is too high, pick another tile.
                            left = !left;
                        }

                        // Place a curve.
                        map.tiles[cursorPosition] = {
                            left ? generator.curveLeftTileScene : generator.curveRightTileScene, cursorRotation};
                        cursorRotation += left ? -1 : 1;
                    }
                    else
                    {
                        // Place a straight tile.
                        map.tiles[cursorPosition] = {generator.straightTileScene, cursorRotation};
                    }

                    cursorPosition += rotationToDirection(cursorRotation);
                }

                // Place the end tile.
                map.tiles[cursorPosition] = {generator.endTileScene, cursorRotation};

                cmds.remove<MapGenerator>(ent);
                cmds.add(ent, map);
            }
        });
}
