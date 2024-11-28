# airships/server/CMakeLists.txt
# Airships server build configuration

add_executable(airships-server
    "src/main.cpp"

    "src/player/plugin.cpp"
    "src/player/player.cpp"

    "src/packets/plugin.cpp"
    "src/packets/out/disconnect/plugin.cpp"
    "src/packets/in/join/plugin.cpp"

    "src/join/plugin.cpp"

    "src/game_server/plugin.cpp"
    "src/game_server/game_server.cpp"
)

target_link_libraries(airships-server airships-common)
common_demo_options(airships-server)

if(DISTRIBUTE)
    target_compile_definitions(airships-server PRIVATE
        APP_ASSETS_PATH="assets"
        BUILTIN_ASSETS_PATH="builtin"
    )
else()
    target_compile_definitions(airships-server PRIVATE
        APP_ASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/../assets"
        BUILTIN_ASSETS_PATH="${CUBOS_ENGINE_ASSETS_PATH}"
    )
endif()

if(DISTRIBUTE)
    install(TARGETS airships EXPORT airships-server RUNTIME DESTINATION airships)
endif()
