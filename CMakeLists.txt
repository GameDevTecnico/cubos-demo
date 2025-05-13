# CMakeLists.txt
# Cubos demo projects root build configuration

cmake_minimum_required(VERSION 3.25.0)
project(cubos-demo VERSION 0.1.0)

find_package(cubos REQUIRED)

option(DISTRIBUTE "Build the games for distribution" OFF)

function(common_demo_options target)
    # Enable all warnings and treat them as errors
    target_compile_options(${target} PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:
            /Zc:preprocessor> # Enable preprocessor conformance mode - required for __VA_ARGS__ to work correctly
    )

    if(EMSCRIPTEN)
        set_target_properties(${target} PROPERTIES SUFFIX ".html")
    endif()
endfunction()

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fsanitize-recover=address")

# Add the subdirectories for each demo project

option(BUILD_CARS "Build legacy cars demo" ON)
message("Building legacy cars demo: ${BUILD_CARS}")
if(BUILD_CARS)
    add_subdirectory(cars)
endif()

option(BUILD_SCRAPS "Build Scraps vs Zombies" ON)
message("Building Scraps vs Zombies: ${BUILD_SCRAPS}")
if(BUILD_SCRAPS)
    add_subdirectory(scraps)
endif()

option(BUILD_AIRSHIPS "Build Airships" ON)
message("Building Airships: ${BUILD_AIRSHIPS}")
if(BUILD_AIRSHIPS)
    add_subdirectory(airships)
endif()

option(BUILD_ONDISSEIA "Build Ondisseia" ON)
message("Building Ondisseia: ${BUILD_ONDISSEIA}")
if(BUILD_ONDISSEIA)
    add_subdirectory(ondisseia)
endif()

option(BUILD_ROLL_RACERS "Build Roll Racers" ON)
message("Building Roll Racers: ${BUILD_ROLL_RACERS}")
if(BUILD_ROLL_RACERS)
    add_subdirectory(roll_racers)
endif()
