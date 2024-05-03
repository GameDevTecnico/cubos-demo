# coffee-jam/CMakeLists.txt
# Coffee Jam demo build configuration

add_executable(coffee-jam
    src/main.cpp
    src/follow/plugin.cpp
    src/tile_map/plugin.cpp
    src/tile_map_generator/plugin.cpp
    src/walker/plugin.cpp
    src/object/plugin.cpp
    src/holdable/plugin.cpp
    src/interaction/plugin.cpp
    src/breakable/plugin.cpp
    src/hammer/plugin.cpp
    src/player_controller/plugin.cpp
    src/turret/plugin.cpp
    src/path_finding/plugin.cpp
    src/zombie/plugin.cpp
    src/health/plugin.cpp
    src/progression/plugin.cpp
    src/displays/plugin.cpp
    src/day_night/plugin.cpp
    src/waves/plugin.cpp
    src/shop/plugin.cpp
    src/player_spawn_point/plugin.cpp
    src/skip/plugin.cpp
)

target_link_libraries(coffee-jam tesseratos)
target_compile_definitions(coffee-jam PUBLIC PROJECT_ASSETS_FOLDER="${CMAKE_CURRENT_SOURCE_DIR}/assets")
target_compile_features(coffee-jam PRIVATE cxx_std_20)
common_demo_options(coffee-jam)
