# airships/CMakeLists.txt
# Airships build configuration

add_subdirectory(common)
add_subdirectory(client)
if(NOT EMSCRIPTEN)
    add_subdirectory(server)
endif()

# ------------------------ Configure game installation ------------------------

if(DISTRIBUTE AND NOT EMSCRIPTEN)
    install(DIRECTORY assets/ DESTINATION airships/assets)
    install(DIRECTORY ${CUBOS_ENGINE_ASSETS_PATH}/ DESTINATION airships/builtin)
endif()
