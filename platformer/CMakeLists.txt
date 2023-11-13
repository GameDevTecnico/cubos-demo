# CMakeLists.txt
# Platformer build configuration

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../lib/cubos/cmake")

include(QuadradosGenerate)

add_executable(platformer
    src/main.cpp
    src/player/plugin.cpp
    src/player/player.cpp
)

target_link_libraries(platformer tesseratos)
target_compile_definitions(platformer PUBLIC PROJECT_ASSETS_FOLDER="${CMAKE_CURRENT_SOURCE_DIR}/assets")
target_compile_features(platformer PRIVATE cxx_std_20)
quadrados_generate(platformer "${CMAKE_CURRENT_SOURCE_DIR}")
