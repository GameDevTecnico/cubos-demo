# tanks/CMakeLists.txt
# Tanks demo build configuration

add_executable(tanks
    src/main.cpp
)

target_link_libraries(tanks tesseratos)
target_compile_definitions(tanks PUBLIC PROJECT_ASSETS_FOLDER="${CMAKE_CURRENT_SOURCE_DIR}/assets")
target_compile_features(tanks PRIVATE cxx_std_20)
