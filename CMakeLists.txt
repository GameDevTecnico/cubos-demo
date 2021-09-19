cmake_minimum_required(VERSION 3.0.0)
project(cubos-demo VERSION 0.1.0)

set(DEMO_SOURCE
    "src/main.cpp"
)

add_executable(demo ${DEMO_SOURCE})

# Dependencies
add_subdirectory(lib/cubos)
target_link_libraries(demo cubos)

# Enable CTest and CPack
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
