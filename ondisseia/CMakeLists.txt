# ondisseia/CMakeLists.txt
# Ondisseia build configuration

add_executable(ondisseia
    src/main.cpp
    src/tile_map/plugin.cpp
    src/tile_map_generator/plugin.cpp
    src/camera_controller/plugin.cpp
    src/waves/plugin.cpp
    src/player_movement/plugin.cpp
    src/player_controller/plugin.cpp
    src/player_spawner/plugin.cpp
    src/waves_animator/plugin.cpp
    src/health/plugin.cpp
    src/animation/plugin.cpp
    src/random_grid/plugin.cpp
    src/bullet/plugin.cpp
    src/destroy_tree/plugin.cpp
    src/sailor_death/plugin.cpp
    src/coordinator/plugin.cpp
    src/scoreboard/plugin.cpp
    src/level/plugin.cpp
    src/numerals/plugin.cpp
    src/score_indicator/plugin.cpp
    src/rotating_animation/plugin.cpp
    src/audio/plugin.cpp
)

target_link_libraries(ondisseia cubos::engine)
target_compile_features(ondisseia PRIVATE cxx_std_20)
common_demo_options(ondisseia)

if(EMSCRIPTEN)
    target_link_options(ondisseia PRIVATE
        "SHELL:--embed-file ${CMAKE_CURRENT_SOURCE_DIR}/assets@/assets"
        "SHELL:--embed-file ${CUBOS_ENGINE_ASSETS_PATH}@/builtin"
        "SHELL:--shell-file ${CMAKE_CURRENT_SOURCE_DIR}/emscripten.html"
    )
    target_compile_definitions(ondisseia PRIVATE
        APP_ASSETS_PATH="/assets"
        BUILTIN_ASSETS_PATH="/builtin"
    )
else()
    if(DISTRIBUTE)
        target_compile_definitions(ondisseia PRIVATE
            APP_ASSETS_PATH="assets"
            BUILTIN_ASSETS_PATH="builtin"
        )
    else()
        target_compile_definitions(ondisseia PRIVATE
            APP_ASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets"
            BUILTIN_ASSETS_PATH="${CUBOS_ENGINE_ASSETS_PATH}"
        )
    endif()
endif()

# ------------------------ Configure game installation ------------------------

if(DISTRIBUTE)
    if(EMSCRIPTEN)
        install(FILES $<TARGET_FILE_DIR:ondisseia>/ondisseia.html DESTINATION ondisseia)
        install(FILES $<TARGET_FILE_DIR:ondisseia>/ondisseia.wasm DESTINATION ondisseia)
        install(FILES $<TARGET_FILE_DIR:ondisseia>/ondisseia.js DESTINATION ondisseia)
    else()
        install(TARGETS ondisseia EXPORT ondisseia RUNTIME DESTINATION ondisseia)
    endif()
    install(DIRECTORY assets/ DESTINATION ondisseia/assets)
    install(DIRECTORY ${CUBOS_ENGINE_ASSETS_PATH}/ DESTINATION ondisseia/builtin)
endif()
