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
        .withField("finishTileScene", &MapGenerator::finishTileScene)
        .withField("endTileScene", &MapGenerator::endTileScene)
        .withField("straightTileScene", &MapGenerator::straightTileScene)
        .withField("straightWithRampTileScene", &MapGenerator::straightWithRampTileScene)
        .withField("curveLeftTileScene", &MapGenerator::curveLeftTileScene)
        .withField("curveRightTileScene", &MapGenerator::curveRightTileScene)
        .withField("orangeBuildingTileScene", &MapGenerator::orangeBuildingTileScene)
        .withField("blackBuildingTileScene", &MapGenerator::blackBuildingTileScene)
        .withField("whiteBuildingTileScene", &MapGenerator::whiteBuildingTileScene)
        .withField("fillerBuildingTileScene", &MapGenerator::fillerBuildingTileScene)
        .withField("marketBuildingTileScene", &MapGenerator::marketBuildingTileScene)
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

    cubos.observer("generate Map").onAdd<MapGenerator>().call([](Commands cmds, Query<Entity, MapGenerator&> query) {
        std::mt19937 rng(std::random_device{}());
        std::uniform_real_distribution<float> chanceDist(0.0F, 1.0F);

        for (auto [ent, generator] : query)
        {
            Map map{};
            map.tileSize = generator.tileSize;

            // List of road tiles by the order they were placed in.
            std::vector<std::pair<glm::ivec2, MapTile>> roadTiles;
            std::vector<glm::ivec2> directions = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};
            std::vector<glm::ivec2> cornerDirections = {{1, 1}, {1, -1}, {-1, -1}, {-1, 1}};
            MapTile newTile;

            // Start by placing the main road.
            map.tiles[{0, -1}] = {generator.finishTileScene, 0};
            roadTiles.push_back({{0, -1}, map.tiles[{0, -1}]});
            map.tiles[{0, 0}] = {generator.startTileScene, 0};
            roadTiles.push_back({{0, 0}, map.tiles[{0, 0}]});
            glm::ivec2 cursorPosition{0, 1};
            int cursorRotation = 0;

            int successiveCurves = 0;
            int successiveStraight = 0;
            bool wasLastCurveLeft = false;
            bool wasHoleBefore = false;
            bool firstTile = true;

            for (std::size_t i = 0; i < generator.trackLength; ++i)
            {
                float curveChance = 0.20F + (0.15F * static_cast<float>(successiveStraight) +
                                             0.5F * static_cast<float>(successiveCurves));
                if (successiveCurves >= 2)
                {
                    // No more than 2 curves in a row.
                    curveChance = 0.0F;
                }
                if (firstTile || wasHoleBefore)
                {
                    // No curves on the first tile or after a hole.
                    curveChance = 0.0F;
                    firstTile = false;
                    wasHoleBefore = false;
                }

                if (chanceDist(rng) < curveChance)
                {
                    successiveStraight = 0;
                    successiveCurves += 1;

                    bool left = chanceDist(rng) < 0.5F;
                    if (successiveCurves == 2)
                    {
                        // If we have two curves in a row, we make sure that the second curve is in the opposite
                        // direction of the first one.
                        left = !wasLastCurveLeft;
                    }
                    if (cursorRotation == -1 && left || cursorRotation == 1 && !left)
                    {
                        // If the rotation is too high, pick another tile.
                        left = !left;
                    }
                    wasLastCurveLeft = left;

                    // Place a curve tile.
                    newTile = {left ? generator.curveLeftTileScene : generator.curveRightTileScene, cursorRotation};
                    cursorRotation += left ? -1 : 1;
                }
                else
                {
                    float holeChance = 0.2F + 0.1F * static_cast<float>(successiveStraight);
                    if (successiveStraight == 0 || wasHoleBefore)
                    {
                        holeChance = 0.0F;
                        wasHoleBefore = false;
                    }

                    successiveStraight += 1;
                    successiveCurves = 0;

                    if (chanceDist(rng) < holeChance)
                    {
                        // Place a straight tile with a hole.
                        wasHoleBefore = true;
                        newTile = {generator.straightWithRampTileScene, cursorRotation};
                    }
                    else
                    {
                        // Place a straight tile.
                        newTile = {generator.straightTileScene, cursorRotation};
                    }
                }

                map.tiles[cursorPosition] = newTile;
                roadTiles.push_back({cursorPosition, newTile});
                cursorPosition += rotationToDirection(cursorRotation);
            }

            // Place the end tile.
            map.tiles[cursorPosition] = {generator.endTileScene, cursorRotation};
            roadTiles.push_back({cursorPosition, map.tiles[cursorPosition]});
            cursorPosition += rotationToDirection(cursorRotation);

            // Place the supermarket building.
            map.tiles[cursorPosition] = {generator.marketBuildingTileScene, cursorRotation};

            // Add surrounding buildings to the road tiles.
            for (const auto& [position, tile] : roadTiles)
            {
                // Check all adjacent directions of the road tile.
                for (int i = 0; i < 4; i++)
                {
                    glm::ivec2 buildingPosition = position + directions[i];

                    if (map.tiles.find(buildingPosition) == map.tiles.end())
                    {
                        float buildingChance = chanceDist(rng);
                        cubos::engine::Asset<cubos::engine::Scene> buildingScene;

                        // Choose random building color.
                        if (buildingChance < 0.63)
                        {
                            buildingScene = generator.orangeBuildingTileScene;
                        }
                        else if (buildingChance < 0.9)
                        {
                            buildingScene = generator.blackBuildingTileScene;
                        }
                        else
                        {
                            buildingScene = generator.whiteBuildingTileScene;
                        }

                        map.tiles[buildingPosition] = {buildingScene, i + 2};
                    }
                }
            }

            // Add filler buildings to the corners of road tiles.
            for (const auto& [position, tile] : roadTiles)
            {
                // Check all corner directions of the road tile.
                for (int i = 0; i < 4; i++)
                {
                    glm::ivec2 fillerPosition = position + cornerDirections[i];
                    if (map.tiles.find(fillerPosition) == map.tiles.end())
                    {
                        map.tiles[fillerPosition] = {generator.fillerBuildingTileScene, 0};
                    }
                }
            }

            cmds.remove<MapGenerator>(ent);
            cmds.add(ent, map);
        }
    });
}
