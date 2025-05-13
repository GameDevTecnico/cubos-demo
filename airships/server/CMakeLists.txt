# airships/server/CMakeLists.txt
# Airships server build configuration

add_executable(airships-server
    src/main.cpp
)
target_link_libraries(airships-server PUBLIC cubos::engine)
target_link_libraries(airships-server PRIVATE airships-common)
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
    install(TARGETS airships-server EXPORT airships-server RUNTIME DESTINATION airships)
endif()
