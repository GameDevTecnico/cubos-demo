#include "plugin.hpp"
#include "../tile_map/plugin.hpp"
#include "../object/plugin.hpp"

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
        .withField("chunkSide", &TileMapGenerator::chunkSide)
        .withField("tileSide", &TileMapGenerator::tileSide)
        .withField("border", &TileMapGenerator::border)
        .withField("grass", &TileMapGenerator::grass)
        .withField("roadDash", &TileMapGenerator::roadDash)
        .withField("roadLine", &TileMapGenerator::roadLine)
        .withField("roadCorner", &TileMapGenerator::roadCorner)
        .withField("roadCurve", &TileMapGenerator::roadCurve)
        .withField("roadTJunction", &TileMapGenerator::roadTJunction)
        .withField("roadJunction", &TileMapGenerator::roadJunction)
        .withField("roadSimple", &TileMapGenerator::roadSimple)
        .withField("sidewalk", &TileMapGenerator::sidewalk)
        .withField("fenceStraight", &TileMapGenerator::fenceStraight)
        .withField("fenceCurve", &TileMapGenerator::fenceCurve)
        .withField("wallStraight", &TileMapGenerator::wallStraight)
        .withField("wallCurve", &TileMapGenerator::wallCurve)
        .withField("crate", &TileMapGenerator::crate)
        .withField("car1", &TileMapGenerator::car1)
        .build();
}

namespace
{
    struct FloorTypes
    {
        unsigned char grass;
        unsigned char roadDash;
        unsigned char roadLine;
        unsigned char roadCorner;
        unsigned char roadCurve;
        unsigned char roadTJunction;
        unsigned char roadJunction;
        unsigned char roadSimple;
        unsigned char sidewalk;
    };

    struct WallTypes
    {
        unsigned char fenceStraight;
        unsigned char fenceCurve;
        unsigned char wallStraight;
        unsigned char wallCurve;
    };

    struct ObjectTypes
    {
        AssetRead<Scene> crate;
        AssetRead<Scene> car1;
    };

    struct Cursor
    {
        Entity mapEnt;
        Commands& cmds;
        const FloorTypes& floors;
        const WallTypes& walls;
        const ObjectTypes& objects;
        demo::Tile& floor;
        demo::Tile& wall;
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

static unsigned char registerFloor(demo::TileMap& tileMap, Asset<VoxelGrid> asset)
{
    tileMap.floorTypes.push_back(asset);
    return static_cast<unsigned char>(tileMap.floorTypes.size() - 1);
}

static unsigned char registerWall(demo::TileMap& tileMap, Asset<VoxelGrid> asset)
{
    tileMap.wallTypes.push_back(asset);
    return static_cast<unsigned char>(tileMap.wallTypes.size() - 1);
}

static bool makeWallBox(const Cursor& cursor, unsigned char edgeType, unsigned char cornerType, Rect rect)
{
    if (cursor.tx == rect.x1 && cursor.ty == rect.y1)
    {
        cursor.wall = {cornerType, 0};
        return true;
    }

    if (cursor.tx == rect.x2 && cursor.ty == rect.y1)
    {
        cursor.wall = {cornerType, 1};
        return true;
    }

    if (cursor.tx == rect.x1 && cursor.ty == rect.y2)
    {
        cursor.wall = {cornerType, 3};
        return true;
    }

    if (cursor.tx == rect.x2 && cursor.ty == rect.y2)
    {
        cursor.wall = {cornerType, 2};
        return true;
    }

    if (cursor.tx > rect.x1 && cursor.tx < rect.x2 && (cursor.ty == rect.y1 || cursor.ty == rect.y2))
    {
        cursor.wall = {edgeType, 0};
        return true;
    }

    if (cursor.ty > rect.y1 && cursor.ty < rect.y2 && (cursor.tx == rect.x1 || cursor.tx == rect.x2))
    {
        cursor.wall = {edgeType, 1};
        return true;
    }

    return false;
}

static bool makeFloorBox(const Cursor& cursor, unsigned char type, unsigned char rotation, Rect rect)
{
    if (rect.contains(cursor.tx, cursor.ty))
    {
        cursor.floor = {type, rotation};
        return true;
    }

    return false;
}

static bool makeRoadX(const Cursor& cursor, Rect rect)
{
    if (cursor.tx >= rect.x1 && cursor.tx <= rect.x2)
    {
        int centerY = (rect.y1 + rect.y2) / 2;
        int radius = rect.sizeY() / 2 - 1;

        if (cursor.ty >= centerY - radius && cursor.ty <= centerY + radius)
        {
            cursor.wall = {UINT8_MAX};
            cursor.floor = {cursor.floors.roadSimple, 0};
            return true;
        }

        if (cursor.ty >= rect.y1 && cursor.ty <= rect.y2)
        {
            cursor.wall = {UINT8_MAX};
            cursor.floor = {cursor.floors.sidewalk, 1};
            return true;
        }
    }

    return false;
}

static void makeParkingLotSection(const Cursor& cursor, Rect totalRect, int placeSizeX, int placeSizeY, int roadWidth)
{
    if (!totalRect.contains(cursor.tx, cursor.ty))
    {
        return;
    }

    CUBOS_ASSERT(totalRect.sizeX() == roadWidth + 2 * placeSizeX + 4);
    CUBOS_ASSERT((totalRect.sizeY() - 1) % (placeSizeY + 1) == 0);
    auto placeCountY = (totalRect.sizeY() - 1) / (placeSizeY + 1);

    if (cursor.tx == totalRect.x1 || cursor.tx == totalRect.x2)
    {
        if (cursor.ty == totalRect.y1)
        {
            cursor.floor = {cursor.floors.roadTJunction, 0};
        }
        else if (cursor.ty == totalRect.y2)
        {
            cursor.floor = {cursor.floors.roadTJunction, 2};
        }
        else if ((cursor.ty - totalRect.y1) % (placeSizeY + 1) == 0)
        {
            cursor.floor = {cursor.floors.roadJunction, 0};
        }
        else
        {
            cursor.floor = {cursor.floors.roadLine, 0};
        }
    }
    else if (cursor.tx == totalRect.x1 + placeSizeX + 1)
    {
        if (cursor.ty == totalRect.y1)
        {
            cursor.floor = {cursor.floors.roadCurve, 0};
        }
        else if (cursor.ty == totalRect.y2)
        {
            cursor.floor = {cursor.floors.roadCurve, 1};
        }
        else if ((cursor.ty - totalRect.y1) % (placeSizeY + 1) == 0)
        {
            cursor.floor = {cursor.floors.roadTJunction, 1};

            if (rand() % 100 <= 25)
            {
                auto car = cursor.cmds.spawn(cursor.objects.car1->blueprint).entity("car");
                cursor.cmds.relate(car, cursor.mapEnt, ChildOf{});
                cursor.cmds.add(car, Object{.position = {cursor.tx - placeSizeX, cursor.ty + 1}, .size = {3, 2}});
            }
        }
    }
    else if (cursor.tx == totalRect.x2 - placeSizeX - 1)
    {
        if (cursor.ty == totalRect.y1)
        {
            cursor.floor = {cursor.floors.roadCurve, 3};
        }
        else if (cursor.ty == totalRect.y2)
        {
            cursor.floor = {cursor.floors.roadCurve, 2};
        }
        else if ((cursor.ty - totalRect.y1) % (placeSizeY + 1) == 0)
        {
            cursor.floor = {cursor.floors.roadTJunction, 3};
        }
    }
    else if ((cursor.tx <= totalRect.x1 + placeSizeX || cursor.tx >= totalRect.x2 - placeSizeX) &&
             ((cursor.ty >= totalRect.y1 && (cursor.ty - totalRect.y1) % (placeSizeY + 1) == 0) ||
              (cursor.ty == totalRect.y2 && (cursor.ty - totalRect.y1) % (placeSizeY + 1) == 0)))
    {
        cursor.floor = {cursor.floors.roadLine, 1};
    }
}

static bool makeParkingLot(const Cursor& cursor, Rect rect)
{
    if (makeWallBox(cursor, cursor.walls.fenceStraight, cursor.walls.fenceCurve, rect))
    {
        cursor.floor = {cursor.floors.sidewalk, 0};
        return true;
    }

    if (!makeFloorBox(cursor, cursor.floors.roadSimple, 0, rect.border(1)))
    {
        return false;
    }

    auto placeSizeX = 3;
    auto placeSizeY = 2;
    auto sideSectionRoadWidth = 3;
    auto sideSectionPlaceCount = 7;
    auto sideSectionLength = sideSectionPlaceCount * placeSizeX + 1;
    auto sideSectionWidth = placeSizeX * 2 + 4 + sideSectionRoadWidth;
    auto sideSectionCount = (rect.sizeX() - 1) / (sideSectionWidth - 1);
    for (int i = 0; i < sideSectionCount; ++i)
    {
        makeParkingLotSection(cursor,
                              Rect{rect.x1 + i * (sideSectionWidth - 1), rect.y1,
                                   rect.x1 + (i + 1) * (sideSectionWidth - 1), rect.y1 + sideSectionLength - 1},
                              placeSizeX, placeSizeY, sideSectionRoadWidth);
        makeParkingLotSection(cursor,
                              Rect{rect.x1 + i * (sideSectionWidth - 1), rect.y2 - sideSectionLength + 1,
                                   rect.x1 + (i + 1) * (sideSectionWidth - 1), rect.y2},
                              placeSizeX, placeSizeY, sideSectionRoadWidth);
    }

    return true;
}

void demo::tileMapGeneratorPlugin(Cubos& cubos)
{
    cubos.depends(tileMapPlugin);
    cubos.depends(assetsPlugin);
    cubos.depends(objectPlugin);
    cubos.depends(transformPlugin);

    cubos.component<TileMapGenerator>();

    cubos.observer("generate TileMap")
        .onAdd<TileMapGenerator>()
        .call([](Commands cmds, Assets& assets, Query<Entity, const TileMapGenerator&> query) {
            for (auto [entity, generator] : query)
            {
                srand(time(nullptr));

                TileMap map{.chunkSide = generator.chunkSide, .tileSide = generator.tileSide};

                FloorTypes floors{};
                floors.grass = registerFloor(map, generator.grass);
                floors.roadDash = registerFloor(map, generator.roadDash);
                floors.roadLine = registerFloor(map, generator.roadLine);
                floors.roadCorner = registerFloor(map, generator.roadCorner);
                floors.roadCurve = registerFloor(map, generator.roadCurve);
                floors.roadTJunction = registerFloor(map, generator.roadTJunction);
                floors.roadJunction = registerFloor(map, generator.roadJunction);
                floors.roadSimple = registerFloor(map, generator.roadSimple);
                floors.sidewalk = registerFloor(map, generator.sidewalk);

                WallTypes walls{};
                walls.fenceStraight = registerWall(map, generator.fenceStraight);
                walls.fenceCurve = registerWall(map, generator.fenceCurve);
                walls.wallStraight = registerWall(map, generator.wallStraight);
                walls.wallCurve = registerWall(map, generator.wallCurve);

                ObjectTypes objects{
                    .crate = assets.read(generator.crate),
                    .car1 = assets.read(generator.car1),
                };

                map.floorTiles.resize(generator.mapSide, std::vector<Tile>(generator.mapSide, Tile{0, 0}));
                map.wallTiles.resize(generator.mapSide, std::vector<Tile>(generator.mapSide, Tile{UINT8_MAX, 0}));

                auto mapRect = Rect::square(generator.mapSide);

                for (int ty = 0; ty < generator.mapSide; ++ty)
                {
                    for (int tx = 0; tx < generator.mapSide; ++tx)
                    {
                        Cursor cursor{
                            .mapEnt = entity,
                            .cmds = cmds,
                            .floors = floors,
                            .walls = walls,
                            .objects = objects,
                            .floor = map.floorTiles[ty][tx],
                            .wall = map.wallTiles[ty][tx],
                            .tx = tx,
                            .ty = ty,
                        };

                        // Figure out the rectangle where the parking lot will be.
                        auto parkingLotRect = mapRect.border(generator.border);
                        parkingLotRect.x2 += 1;
                        parkingLotRect.y1 += 1;
                        makeParkingLot(cursor, parkingLotRect);

                        // Make a road which enters the parking lot.
                        int parkingLotCenterY = (parkingLotRect.y1 + parkingLotRect.y2) / 2;
                        makeRoadX(cursor, Rect{0, parkingLotCenterY - 2, parkingLotRect.x1, parkingLotCenterY + 2});
                    }
                }

                cmds.add(entity, std::move(map));
                cmds.remove<TileMapGenerator>(entity);
            }
        });
}
