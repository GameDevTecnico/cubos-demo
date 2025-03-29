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
        .withField("tileSide", &TileMapGenerator::tileSide)
        .withField("tileHeight", &TileMapGenerator::tileHeight)
        .withField("grass", &TileMapGenerator::grass)
        .withField("sand", &TileMapGenerator::sand)
        .withField("wavesAnimator", &TileMapGenerator::wavesAnimator)
        .build();
}

namespace
{
    struct Types
    {
        unsigned char grass;
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

static unsigned char registerTile(demo::TileMap& tileMap, Asset<VoxelGrid> asset)
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

                VoxelGrid grass{{8, 8, 8}};
                VoxelGrid sand{{8, 8, 8}};

                for (std::size_t x = 0; x < 8; ++x)
                {
                    for (std::size_t y = 0; y < 8; ++y)
                    {
                        for (std::size_t z = 0; z < 8; ++z)
                        {
                            grass.set({x, y, z}, 1);
                            sand.set({x, y, z}, 2);
                        }
                    }
                }

                TileMap map{.tileSide = generator.tileSide, .tileHeight = generator.tileHeight};

                Types types{};
                types.grass = registerTile(map, assets.create(std::move(grass)));
                types.sand = registerTile(map, assets.create(std::move(sand)));

                map.tiles.resize(generator.mapSide, std::vector<Tile>(generator.mapSide, Tile{0, 0}));

                auto mapRect = Rect::square(generator.mapSide);

                std::vector<glm::vec2> landPoints = {};
                for (int i = 0; i < 5; ++i)
                {
                    landPoints.push_back({static_cast<float>(rand() % (generator.mapSide - 4) + 2),
                                          static_cast<float>(rand() % (generator.mapSide - 4) + 2)});
                }

                for (int ty = 0; ty < generator.mapSide; ++ty)
                {
                    for (int tx = 0; tx < generator.mapSide; ++tx)
                    {
                        Cursor cursor{
                            .mapEnt = entity,
                            .cmds = cmds,
                            .types = types,
                            .tile = map.tiles[ty][tx],
                            .tx = tx,
                            .ty = ty,
                        };

                        float distanceToIsland = INFINITY;
                        for (const auto& point : landPoints)
                        {
                            float distance =
                                glm::distance(point, glm::vec2(static_cast<float>(tx), static_cast<float>(ty)));
                            distanceToIsland = glm::min(distanceToIsland, distance);
                        }

                        float mapDiagonal = glm::sqrt(static_cast<float>(map.tileSide * map.tileSide));
                        float islandFactor = 1.0F - distanceToIsland / mapDiagonal;
                        islandFactor = glm::clamp(islandFactor, 0.0F, 1.0F);

                        // Apply some noise to the island factor
                        islandFactor *= (rand() % 2) ? 1.0 : 0.8;

                        cursor.tile.height = static_cast<unsigned char>(glm::round(islandFactor * 8.0F));

                        if (islandFactor > 0.75F)
                        {
                            cursor.tile.type = types.grass;
                        }
                        else
                        {
                            cursor.tile.type = types.sand;
                        }
                    }
                }

                cmds.add(entity, std::move(map));
                cmds.add(entity, Waves{});
                cmds.add(entity, generator.wavesAnimator);
                cmds.remove<TileMapGenerator>(entity);
            }
        });
}
