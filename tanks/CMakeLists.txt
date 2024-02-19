# tanks/CMakeLists.txt
# Tanks demo build configuration

add_executable(tanks
    src/main.cpp
    src/health/death.cpp
    src/health/health.cpp
    src/health/plugin.cpp
    src/bullet/bullet.cpp
    src/bullet/plugin.cpp
    src/gun/gun.cpp
    src/gun/plugin.cpp
    src/tank/tank.cpp
    src/tank/plugin.cpp
    src/particles/particle.cpp
    src/particles/gun_shoot_explosion.cpp
    src/particles/bullet_hit_explosion.cpp
    src/particles/plugin.cpp
)

target_link_libraries(tanks tesseratos)
target_compile_definitions(tanks PUBLIC PROJECT_ASSETS_FOLDER="${CMAKE_CURRENT_SOURCE_DIR}/assets")
target_compile_features(tanks PRIVATE cxx_std_20)
common_demo_options(tanks)
