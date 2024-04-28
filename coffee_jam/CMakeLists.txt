# coffee-jam/CMakeLists.txt
# Coffee Jam demo build configuration

add_executable(coffee-jam
    src/main.cpp
    src/follow/plugin.cpp
    src/tiles/plugin.cpp
)

target_link_libraries(coffee-jam tesseratos)
target_compile_definitions(coffee-jam PUBLIC PROJECT_ASSETS_FOLDER="${CMAKE_CURRENT_SOURCE_DIR}/assets")
target_compile_features(coffee-jam PRIVATE cxx_std_20)
common_demo_options(coffee-jam)
