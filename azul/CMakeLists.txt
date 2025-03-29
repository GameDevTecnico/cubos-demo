# azul/CMakeLists.txt
# Azul jam build configuration

add_executable(azul
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
)

target_link_libraries(azul cubos::engine)
target_compile_features(azul PRIVATE cxx_std_20)
common_demo_options(azul)

if(EMSCRIPTEN)
    target_link_options(azul PRIVATE
        "SHELL:--embed-file ${CMAKE_CURRENT_SOURCE_DIR}/assets@/assets"
        "SHELL:--embed-file ${CUBOS_ENGINE_ASSETS_PATH}@/builtin"
    )
    target_compile_definitions(azul PRIVATE
        APP_ASSETS_PATH="/assets"
        BUILTIN_ASSETS_PATH="/builtin"
    )
else()
    if(DISTRIBUTE)
        target_compile_definitions(azul PRIVATE
            APP_ASSETS_PATH="assets"
            BUILTIN_ASSETS_PATH="builtin"
        )
    else()
        target_compile_definitions(azul PRIVATE
            APP_ASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets"
            BUILTIN_ASSETS_PATH="${CUBOS_ENGINE_ASSETS_PATH}"
        )
    endif()
endif()

# ------------------------ Configure game installation ------------------------

if(DISTRIBUTE)
    install(TARGETS azul EXPORT azul RUNTIME DESTINATION azul)
    install(DIRECTORY assets/ DESTINATION azul/assets)
    install(DIRECTORY ${CUBOS_ENGINE_ASSETS_PATH}/ DESTINATION azul/builtin)
endif()
