# cars/CMakeLists.txt
# Cars demo build configuration

add_executable(cars
    src/car/car.cpp
    src/car/plugin.cpp

    src/day_night/plugin.cpp

    src/explosion/explosion.cpp
    src/explosion/particle.cpp
    src/explosion/plugin.cpp

    src/race/checkpoint.cpp
    src/race/obstacle.cpp
    src/race/plugin.cpp
    src/race/racer.cpp

    src/dead/dead.cpp
    src/dead/plugin.cpp

    src/main.cpp
)

target_link_libraries(cars tesseratos)
target_compile_definitions(cars PUBLIC PROJECT_ASSETS_FOLDER="${CMAKE_CURRENT_SOURCE_DIR}/assets")
target_compile_features(cars PRIVATE cxx_std_20)
common_demo_options(cars)
