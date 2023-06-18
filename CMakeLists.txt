# CMakeLists.txt
# Cubos Demo project root build configuration

cmake_minimum_required(VERSION 3.20.0)
project(cubos-demo VERSION 0.1.0)

# Include CubinhosGenerate
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lib/cubos/cmake")
include(CubinhosGenerate)

# Build demo executabe
set(DEMO_SOURCE "src/main.cpp" "src/explosion.cpp")
add_executable(demo ${DEMO_SOURCE})

# Dependencies
add_subdirectory(lib/cubos)
target_link_libraries(demo cubos-engine)

# Assets
target_compile_definitions(demo PUBLIC DEMO_ASSETS_FOLDER="${CMAKE_CURRENT_SOURCE_DIR}/src/assets")

target_compile_features(demo PRIVATE cxx_std_20)

# Components
cubinhos_generate(demo "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Enable CTest and CPack
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
