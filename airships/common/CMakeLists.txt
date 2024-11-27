# airships/common/CMakeLists.txt
# Airships common library build configuration

add_library(airships-common
    src/empty.cpp
)

target_link_libraries(airships-common PUBLIC cubos::engine)
target_include_directories(airships-common PUBLIC include/)
target_compile_features(airships-common PUBLIC cxx_std_20)
common_demo_options(airships-common)
