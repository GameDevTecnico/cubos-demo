# CMakeLists.txt
# Cubos Demo project root build configuration

cmake_minimum_required(VERSION 3.20.0)
project(cubos-demo VERSION 0.1.0)

add_subdirectory(lib/cubos)

function(common_demo_options target)
    # Enable all warnings and treat them as errors
    target_compile_options(${target} PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:
            /Zc:preprocessor> # Enable preprocessor conformance mode - required for __VA_ARGS__ to work correctly
    )
endfunction()

# TODO: update cars to cubos 0.1 and uncomment this
add_subdirectory(cars)
add_subdirectory(tanks)
