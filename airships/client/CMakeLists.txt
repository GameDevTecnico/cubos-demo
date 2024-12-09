# airships/client/CMakeLists.txt
# Airships client build configuration

add_executable(airships-client
    src/main.cpp 
    src/islands/plugin.cpp
    src/balloons/plugin.cpp
    src/storm/plugin.cpp
    src/random_position/plugin.cpp
    src/drivable/plugin.cpp
    src/steering_wheel/plugin.cpp
    src/rudder/plugin.cpp
    src/follow/plugin.cpp
    src/follow_controller/plugin.cpp
    src/cannon/plugin.cpp
    src/player/plugin.cpp
    src/interactable/plugin.cpp
    src/animation/plugin.cpp
    src/hide/plugin.cpp
    src/rope/plugin.cpp
    src/see_through/plugin.cpp
    src/furnace/plugin.cpp
    src/interpolation/plugin.cpp
    src/auto_draws_to/plugin.cpp
)
target_link_libraries(airships-client PUBLIC cubos::engine)
target_link_libraries(airships-client PRIVATE airships-common)
common_demo_options(airships-client)

if(DISTRIBUTE)
    target_compile_definitions(airships-client PRIVATE
        APP_ASSETS_PATH="assets"
        BUILTIN_ASSETS_PATH="builtin"
    )
else()
    target_compile_definitions(airships-client PRIVATE
        APP_ASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/../assets"
        BUILTIN_ASSETS_PATH="${CUBOS_ENGINE_ASSETS_PATH}"
    )
endif()

if(DISTRIBUTE)
    install(TARGETS airships EXPORT airships-client RUNTIME DESTINATION airships)
endif()
