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

target_link_libraries(cars cubos::engine)
target_compile_features(cars PRIVATE cxx_std_20)
common_demo_options(cars)

if(EMSCRIPTEN)
    target_link_options(cars PRIVATE
        "SHELL:--embed-file ${CMAKE_CURRENT_SOURCE_DIR}/assets@/assets"
        "SHELL:--embed-file ${CUBOS_ENGINE_ASSETS_PATH}@/builtin"
    )
    target_compile_definitions(cars PRIVATE
        APP_ASSETS_PATH="/assets"
        BUILTIN_ASSETS_PATH="/builtin"
    )
else()
    if(DISTRIBUTE)
        target_compile_definitions(cars PRIVATE
            APP_ASSETS_PATH="assets"
            BUILTIN_ASSETS_PATH="builtin"
        )
    else()
        target_compile_definitions(cars PRIVATE
            APP_ASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets"
            BUILTIN_ASSETS_PATH="${CUBOS_ENGINE_ASSETS_PATH}"
        )
    endif()
endif()

# ------------------------ Configure game installation ------------------------

if(DISTRIBUTE)
    if(EMSCRIPTEN)
        install(FILES $<TARGET_FILE_DIR:cars>/cars.html DESTINATION cars)
        install(FILES $<TARGET_FILE_DIR:cars>/cars.wasm DESTINATION cars)
        install(FILES $<TARGET_FILE_DIR:cars>/cars.js DESTINATION cars)
    else()
        install(TARGETS cars EXPORT cars RUNTIME DESTINATION cars)
    endif()
    install(DIRECTORY assets/ DESTINATION cars/assets)
    install(DIRECTORY ${CUBOS_ENGINE_ASSETS_PATH}/ DESTINATION cars/builtin)
endif()
