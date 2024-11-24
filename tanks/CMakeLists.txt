# tanks/CMakeLists.txt
# Tanks demo build configuration

add_executable(tanks
    src/main.cpp
    src/player/player.cpp
    src/player/plugin.cpp
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

target_link_libraries(tanks cubos::engine)
target_compile_features(tanks PRIVATE cxx_std_20)
common_demo_options(tanks)

if(DISTRIBUTE)
    target_compile_definitions(tanks PRIVATE
        APP_ASSETS_PATH="assets"
        BUILTIN_ASSETS_PATH="builtin"
    )
else()
    target_compile_definitions(tanks PRIVATE
        APP_ASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets"
        BUILTIN_ASSETS_PATH="${CUBOS_ENGINE_ASSETS_PATH}"
    )
endif()
