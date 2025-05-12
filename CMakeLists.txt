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

#add_subdirectory(cars)
#add_subdirectory(tanks)
#add_subdirectory(scraps)
#add_subdirectory(airships)
add_subdirectory(ondisseia)
add_subdirectory(roll_racers)
