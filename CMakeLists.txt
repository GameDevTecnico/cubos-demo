# CMakeLists.txt
# Cubos Demo project root build configuration

cmake_minimum_required(VERSION 3.20.0)
project(cubos-demo VERSION 0.1.0)

add_subdirectory(lib/cubos)

# TODO: update cars to cubos 0.1 and uncomment this
# add_subdirectory(cars)
add_subdirectory(tanks)
