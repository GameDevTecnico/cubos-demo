# CMakeLists.txt
# Cubos Demo project root build configuration

cmake_minimum_required(VERSION 3.20.0)
project(cubos-platformer-demo VERSION 0.1.0)

# Include CubinhosGenerate
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../lib/cubos/cmake")
include(CubinhosGenerate)

# Build demo executabe
set(PROJECT_SOURCE "main.cpp")
add_executable(platformer_demo ${PROJECT_SOURCE})

# Dependencies
add_subdirectory(../lib/cubos lib/cubos)
target_link_libraries(platformer_demo cubos-engine)

# Assets
target_compile_definitions(platformer_demo PUBLIC PROJECT_ASSETS_FOLDER="${CMAKE_CURRENT_SOURCE_DIR}/assets")

target_compile_features(platformer_demo PRIVATE cxx_std_20)

# Components
cubinhos_generate(platformer_demo "${CMAKE_CURRENT_SOURCE_DIR}")

# Enable CTest and CPack
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
