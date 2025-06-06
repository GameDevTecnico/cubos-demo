# scraps/CMakeLists.txt
# Scraps vs Zombies build configuration

add_executable(scraps
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

target_link_libraries(scraps cubos::engine)
target_compile_features(scraps PRIVATE cxx_std_20)
common_demo_options(scraps)

if(EMSCRIPTEN)
    target_link_options(scraps PRIVATE
        "SHELL:--embed-file ${CMAKE_CURRENT_SOURCE_DIR}/assets@/assets"
        "SHELL:--embed-file ${CUBOS_ENGINE_ASSETS_PATH}@/builtin"
    )
    target_compile_definitions(scraps PRIVATE
        APP_ASSETS_PATH="/assets"
        BUILTIN_ASSETS_PATH="/builtin"
    )
else()
    if(DISTRIBUTE)
        target_compile_definitions(scraps PRIVATE
            APP_ASSETS_PATH="assets"
            BUILTIN_ASSETS_PATH="builtin"
        )
    else()
        target_compile_definitions(scraps PRIVATE
            APP_ASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets"
            BUILTIN_ASSETS_PATH="${CUBOS_ENGINE_ASSETS_PATH}"
        )
    endif()
endif()

# ------------------------ Configure game installation ------------------------

if(DISTRIBUTE)
    if(EMSCRIPTEN)
        install(FILES $<TARGET_FILE_DIR:scraps>/scraps.html DESTINATION scraps)
        install(FILES $<TARGET_FILE_DIR:scraps>/scraps.wasm DESTINATION scraps)
        install(FILES $<TARGET_FILE_DIR:scraps>/scraps.js DESTINATION scraps)
    else()
        install(TARGETS scraps EXPORT scraps RUNTIME DESTINATION scraps)
    endif()
    install(DIRECTORY assets/ DESTINATION scraps/assets)
    install(DIRECTORY ${CUBOS_ENGINE_ASSETS_PATH}/ DESTINATION scraps/builtin)
endif()
