# airships/CMakeLists.txt
# Airships build configuration

add_subdirectory(common)
add_subdirectory(client)
add_subdirectory(server)

# ------------------------ Configure game installation ------------------------

if(DISTRIBUTE)
    install(DIRECTORY assets/ DESTINATION airships/assets)
    install(DIRECTORY ${CUBOS_ENGINE_ASSETS_PATH}/ DESTINATION airships/builtin)
endif()
