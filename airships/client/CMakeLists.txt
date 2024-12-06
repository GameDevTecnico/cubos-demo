# airships/client/CMakeLists.txt
# Airships client build configuration

add_executable(airships-client
    src/main.cpp
    src/menu/plugin.cpp
)

target_link_libraries(airships-client airships-common)
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
