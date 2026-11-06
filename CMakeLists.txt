cmake_minimum_required(VERSION 4.3)
project(Missile_Command LANGUAGES CXX)


include(FetchContent)
FetchContent_Declare(raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 6.2
)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(raylib)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Missile_Command
        src/game_types.h
        src/math2d.h
        src/game.h
        src/frame_loop.h
        src/renderer.h
        src/defenses.cpp
        src/game.cpp
        src/frame_loop.cpp
        src/game_types.cpp
        src/math2d.cpp
        src/renderer.cpp
        src/main.cpp
        src/missiles.h
        src/missiles.cpp
        src/explosions.h
        src/explosions.cpp)

target_link_libraries(Missile_Command PRIVATE raylib)

target_compile_options(Missile_Command PRIVATE
        -Wall
        -Wextra
)

if(EMSCRIPTEN)
    set_target_properties(Missile_Command PROPERTIES
            SUFFIX ".html"
    )

    target_link_options(Missile_Command PRIVATE
            -sUSE_GLFW=3
            -sALLOW_MEMORY_GROWTH=1
    )
endif()