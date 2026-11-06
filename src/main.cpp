//
// Created by t.laurent on 02/11/2026.
//
#include "frame_loop.h"
#include "renderer.h"
#include "raylib.h"
#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

static missile_command::Input collect_input()
{
    missile_command::Input input = {};
    Vector2 mouse = GetMousePosition();
    input.aim = {mouse.x, mouse.y};

    input.fire[0] =
        IsKeyPressed(KEY_Q) || IsKeyPressed(KEY_A) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    input.fire[1] = IsKeyPressed(KEY_S) || IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE);
    input.fire[2] = IsKeyPressed(KEY_D) || IsMouseButtonPressed(MOUSE_BUTTON_RIGHT);
    input.start = IsKeyPressed(KEY_ENTER);
    input.pause = IsKeyPressed(KEY_P);
    return input;
}
static void update_draw_frame(void* argument)
{
    missile_command::FrameState& frame = *static_cast<missile_command::FrameState*>(argument);
    missile_command::Input input = collect_input();
    missile_command::frame_advance(frame, static_cast<double>(GetFrameTime()), input);
    missile_command::render_step(frame.snapshot, frame.input_overflow);
}

int main()
{
    InitWindow(missile_command::SCREEN_WIDTH, missile_command::SCREEN_HEIGHT, "Missile Command");
    // Persistent state and snapshot live outside the constrained web stack.
    static missile_command::FrameState frame;
    missile_command::frame_init(frame);
    #if defined(PLATFORM_WEB)
    emscripten_set_main_loop_arg(update_draw_frame, &frame, 0, 1);
    #else
    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        update_draw_frame(&frame);
    }
    #endif
    CloseWindow();
    return 0;
}