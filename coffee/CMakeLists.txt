# coffee/CMakeLists.txt
# Coffee N' Jam 2025 build configuration

add_executable(coffee
    src/main.cpp
    src/progression/plugin.cpp
    src/day_night/plugin.cpp
    src/map/plugin.cpp
    src/map_generator/plugin.cpp
    src/ui_effects/plugin.cpp
    src/car/plugin.cpp
    src/follow/plugin.cpp
    src/interpolation/plugin.cpp
    src/toilet_paper/plugin.cpp
)

target_link_libraries(coffee cubos::engine)
target_compile_features(coffee PRIVATE cxx_std_20)
common_demo_options(coffee)

if(EMSCRIPTEN)
    target_link_options(coffee PRIVATE
        "SHELL:--embed-file ${CMAKE_CURRENT_SOURCE_DIR}/assets@/assets"
        "SHELL:--embed-file ${CUBOS_ENGINE_ASSETS_PATH}@/builtin"
        "SHELL:--shell-file ${CMAKE_CURRENT_SOURCE_DIR}/emscripten.html"
    )
    target_compile_definitions(coffee PRIVATE
        APP_ASSETS_PATH="/assets"
        BUILTIN_ASSETS_PATH="/builtin"
    )
else()
    if(DISTRIBUTE)
        target_compile_definitions(coffee PRIVATE
            APP_ASSETS_PATH="assets"
            BUILTIN_ASSETS_PATH="builtin"
        )
    else()
        target_compile_definitions(coffee PRIVATE
            APP_ASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets"
            BUILTIN_ASSETS_PATH="${CUBOS_ENGINE_ASSETS_PATH}"
        )
    endif()
endif()

# ------------------------ Configure game installation ------------------------

if(DISTRIBUTE)
    if(EMSCRIPTEN)
        install(FILES $<TARGET_FILE_DIR:coffee>/coffee.html DESTINATION coffee)
        install(FILES $<TARGET_FILE_DIR:coffee>/coffee.wasm DESTINATION coffee)
        install(FILES $<TARGET_FILE_DIR:coffee>/coffee.js DESTINATION coffee)
        install(FILES $<TARGET_FILE_DIR:coffee>/coffee.worker.js DESTINATION coffee)
    else()
        install(TARGETS coffee EXPORT coffee RUNTIME DESTINATION coffee)
    endif()
    install(DIRECTORY assets/ DESTINATION coffee/assets)
    install(DIRECTORY ${CUBOS_ENGINE_ASSETS_PATH}/ DESTINATION coffee/builtin)
endif()
