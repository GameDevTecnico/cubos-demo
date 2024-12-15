# airships/client/CMakeLists.txt
# Airships client build configuration

add_executable(airships-client
    src/main.cpp 
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
    src/player_id/plugin.cpp
    src/interactable/plugin.cpp
    src/animation/plugin.cpp
    src/hide/plugin.cpp
    src/rope/plugin.cpp
    src/see_through/plugin.cpp
    src/furnace/plugin.cpp
    src/interpolation/plugin.cpp
    src/auto_draws_to/plugin.cpp
    src/auto_child_of/plugin.cpp
    src/main_menu/plugin.cpp
    src/level_generator/plugin.cpp
    src/team_spawner/plugin.cpp
    src/resource/plugin.cpp
    src/holdable/plugin.cpp
    src/box/plugin.cpp
    src/damageable/plugin.cpp
    src/render_damage/plugin.cpp
    src/bullet/plugin.cpp
    src/destroy_tree/plugin.cpp
    src/drivable_destruction/plugin.cpp
    src/boat_skin/plugin.cpp
)
target_link_libraries(airships-client PUBLIC cubos::engine)
target_link_libraries(airships-client PRIVATE airships-common)
common_demo_options(airships-client)

if(EMSCRIPTEN)
    target_link_options(airships-client PRIVATE
        "SHELL:--embed-file ${CMAKE_CURRENT_SOURCE_DIR}/../assets@/assets"
        "SHELL:--embed-file ${CUBOS_ENGINE_ASSETS_PATH}@/builtin"
    )
    target_compile_definitions(airships-client PRIVATE
        APP_ASSETS_PATH="/assets"
        BUILTIN_ASSETS_PATH="/builtin"
    )
else()
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
endif()

if(DISTRIBUTE)
    install(TARGETS airships EXPORT airships-client RUNTIME DESTINATION airships)
endif()
