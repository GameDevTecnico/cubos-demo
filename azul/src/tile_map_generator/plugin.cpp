#include "plugin.hpp"
#include "../tile_map/plugin.hpp"
#include "../waves/plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>

#include <ctime>
#include <random>

using namespace cubos::engine;
using namespace demo;

CUBOS_REFLECT_IMPL(demo::TileMapGenerator)
{
    return cubos::core::ecs::TypeBuilder<TileMapGenerator>("demo::TileMapGenerator")
        .withField("mapSide", &TileMapGenerator::mapSide)
        .withField("grass", &TileMapGenerator::grass)
        .withField("mountain", &TileMapGenerator::mountain)
        .withField("sand", &TileMapGenerator::sand)
        .withField("waves", &TileMapGenerator::waves)
        .withField("wavesAnimator", &TileMapGenerator::wavesAnimator)
        .build();
}

namespace
{
    struct Types
    {
        unsigned char grass;
        unsigned char mountain;
        unsigned char sand;
    };

    struct Cursor
    {
        Entity mapEnt;
        Commands& cmds;
        const Types& types;
        demo::Tile& tile;
        int tx;
        int ty;
    };

    struct Rect
    {
        int x1;
        int y1;
        int x2;
        int y2;

        static Rect square(int size)
        {
            return {0, 0, size - 1, size - 1};
        }

        static Rect lineX(int x1, int x2, int y)
        {
            return {x1, y, x2, y};
        }

        static Rect lineY(int y1, int y2, int x)
        {
            return {x, y1, x, y2};
        }

        Rect offset(int dx, int dy) const
        {
            return {x1 + dx, y1 + dy, x2 + dx, y2 + dy};
        }

        Rect border(int size) const
        {
            CUBOS_ASSERT(x2 - x1 >= 2 * size && y2 - y1 >= 2 * size, "Border size is too large for the rectangle");
            return {x1 + size, y1 + size, x2 - size, y2 - size};
        }

        bool contains(int x, int y) const
        {
            return x >= x1 && x <= x2 && y >= y1 && y <= y2;
        }

        bool isLineX() const
        {
            return y1 == y2;
        }

        bool isLineY() const
        {
            return x1 == x2;
        }

        bool isEdge(int x, int y) const
        {
            return (x == x1 || x == x2) && (y == y1 || y == y2);
        }

        int sizeX() const
        {
            return x2 - x1 + 1;
        }

        int sizeY() const
        {
            return y2 - y1 + 1;
        }
    };
} // namespace

static unsigned char registerTile(demo::TileMap& tileMap, Asset<Scene> asset)
{
    tileMap.types.push_back(asset);
    return static_cast<unsigned char>(tileMap.types.size() - 1);
}

void demo::tileMapGeneratorPlugin(Cubos& cubos)
{
    cubos.depends(tileMapPlugin);
    cubos.depends(assetsPlugin);
    cubos.depends(transformPlugin);
    cubos.depends(wavesPlugin);
    cubos.depends(wavesAnimatorPlugin);

    cubos.component<TileMapGenerator>();

    cubos.observer("generate TileMap")
        .onAdd<TileMapGenerator>()
        .call([](Commands cmds, Assets& assets, Query<Entity, const TileMapGenerator&> query) {
            for (auto [entity, generator] : query)
            {
                srand(time(nullptr));

                Waves waves = generator.waves;
                TileMap map{};

                Types types{};
                types.grass = registerTile(map, generator.grass);
                types.mountain = registerTile(map, generator.mountain);
                types.sand = registerTile(map, generator.sand);

                map.tiles.resize(generator.mapSide, std::vector<Tile>(generator.mapSide, Tile{0, 0}));
                waves.terrain.resize(generator.mapSide, std::vector<float>(generator.mapSide, 0.0F));

                auto mapRect = Rect::square(generator.mapSide);

                std::vector<glm::vec2> landPoints = {
                    {5, 5},
                    {generator.mapSide - 5, 5},
                    {5, generator.mapSide - 5},
                    {generator.mapSide - 5, generator.mapSide - 5},
                };
                for (int i = 0; i < 4; ++i)
                {
                    auto offsetX = static_cast<float>(rand() % 6 - 3);
                    auto offsetY = static_cast<float>(rand() % 6 - 3);
                    landPoints[i] += glm::vec2(offsetX, offsetY);
                }

                for (int ty = 0; ty < generator.mapSide; ++ty)
                {
                    for (int tx = 0; tx < generator.mapSide; ++tx)
                    {
                        auto& tile = map.tiles[ty][tx];

                        float distanceToIsland = INFINITY;
                        for (const auto& point : landPoints)
                        {
                            float distance =
                                glm::distance(point, glm::vec2(static_cast<float>(tx), static_cast<float>(ty)));
                            distanceToIsland = glm::min(distanceToIsland, distance);
                        }

                        float mapDiagonal = glm::sqrt(static_cast<float>(2 * generator.mapSide * generator.mapSide));

                        float distanceToMapEdge =
                            glm::min(glm::min(tx, ty), glm::min(generator.mapSide - tx, generator.mapSide - ty));
                        float centerFactor = distanceToMapEdge / mapDiagonal;
                        centerFactor = glm::pow(centerFactor, 0.15F);

                        float islandFactor = 1.0F - distanceToIsland / mapDiagonal;
                        islandFactor = glm::clamp(islandFactor, 0.0F, 1.0F);
                        islandFactor = glm::pow(islandFactor, 3.0F);
                        islandFactor = islandFactor * centerFactor;

                        if (islandFactor < 0.6)
                        {
                            tile.height = 0;
                            tile.blockHeight = 0.5F;
                            tile.type = types.sand;
                        }
                        else if (islandFactor < 0.65)
                        {
                            tile.height = 1;
                            tile.blockHeight = 1.0F;
                            tile.type = types.sand;
                        }
                        else if (islandFactor < 0.675)
                        {
                            tile.height = 1;
                            tile.blockHeight = 1.0F;
                            tile.type = types.grass;
                        }
                        else if (islandFactor < 0.7)
                        {
                            tile.height = 2;
                            tile.blockHeight = 1.5F;
                            tile.type = types.grass;
                        }
                        else
                        {
                            tile.height = 2;
                            tile.blockHeight = 2.0F;
                            tile.type = types.mountain;
                        }
                        tile.rotation = rand() % 4;
                    }
                }

                for (size_t y = 0; y < map.tiles.size(); ++y)
                {
                    for (size_t x = 0; x < map.tiles[y].size(); ++x)
                    {
                        waves.terrain[y][x] = static_cast<float>(map.tiles[y][x].height) * 0.5F + 1;
                    }
                }

                cmds.add(entity, std::move(map));
                cmds.add(entity, waves);
                cmds.add(entity, generator.wavesAnimator);
                cmds.remove<TileMapGenerator>(entity);
            }
        });
}
