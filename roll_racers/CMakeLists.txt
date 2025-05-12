# roll_racers/CMakeLists.txt
# Roll Racers build configuration

add_executable(roll_racers
    src/main.cpp
    src/day_night/plugin.cpp
    src/map/plugin.cpp
    src/map_generator/plugin.cpp
    src/ui_effects/plugin.cpp
    src/car/plugin.cpp
    src/follow/plugin.cpp
    src/interpolation/plugin.cpp
    src/toilet_paper/plugin.cpp
    src/player_spawner/plugin.cpp
    src/blackout/plugin.cpp
    src/spawn_orphan/plugin.cpp
    src/hover_animation/plugin.cpp
    src/round_manager/plugin.cpp
    src/score/plugin.cpp
)

target_link_libraries(roll_racers cubos::engine)
target_compile_features(roll_racers PRIVATE cxx_std_20)
common_demo_options(roll_racers)

if(EMSCRIPTEN)
    target_link_options(roll_racers PRIVATE
        "SHELL:--embed-file ${CMAKE_CURRENT_SOURCE_DIR}/assets@/assets"
        "SHELL:--embed-file ${CUBOS_ENGINE_ASSETS_PATH}@/builtin"
        "SHELL:--shell-file ${CMAKE_CURRENT_SOURCE_DIR}/emscripten.html"
    )
    target_compile_definitions(roll_racers PRIVATE
        APP_ASSETS_PATH="/assets"
        BUILTIN_ASSETS_PATH="/builtin"
    )
else()
    if(DISTRIBUTE)
        target_compile_definitions(roll_racers PRIVATE
            APP_ASSETS_PATH="assets"
            BUILTIN_ASSETS_PATH="builtin"
        )
    else()
        target_compile_definitions(roll_racers PRIVATE
            APP_ASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets"
            BUILTIN_ASSETS_PATH="${CUBOS_ENGINE_ASSETS_PATH}"
        )
    endif()
endif()

# ------------------------ Configure game installation ------------------------

if(DISTRIBUTE)
    if(EMSCRIPTEN)
        install(FILES $<TARGET_FILE_DIR:roll_racers>/roll_racers.html DESTINATION roll_racers)
        install(FILES $<TARGET_FILE_DIR:roll_racers>/roll_racers.wasm DESTINATION roll_racers)
        install(FILES $<TARGET_FILE_DIR:roll_racers>/roll_racers.js DESTINATION roll_racers)
    else()
        install(TARGETS roll_racers EXPORT roll_racers RUNTIME DESTINATION roll_racers)
    endif()
    install(DIRECTORY assets/ DESTINATION roll_racers/assets)
    install(DIRECTORY ${CUBOS_ENGINE_ASSETS_PATH}/ DESTINATION roll_racers/builtin)
endif()
