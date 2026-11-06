//
// Created by t.laurent on 02/11/2026.
//
#include "frame_loop.h"
#include "game.h"

namespace missile_command
{
    bool input_push(InputBuffer& buffer, const Input& input)
    {
        buffer.aim = input.aim;
        bool event = input.start || input.pause;
        for (bool fire : input.fire) {
            event = event || fire;
        }
        if (!event)
        {
            return true;
        }
        if (buffer.count == INPUT_CAPACITY)
        {
            return false;
        }
        int tail = (buffer.head + buffer.count) % INPUT_CAPACITY;
        buffer.items[tail] = input;
        buffer.count++;
        return true;
    }

    Input input_pop(InputBuffer& buffer)
    {
        Input input = {};
        input.aim = buffer.aim;
        if (buffer.count > 0)
        {
            input = buffer.items[buffer.head];
            buffer.head = (buffer.head + 1) % INPUT_CAPACITY;
            buffer.count--;
        }
        return input;
    }

    void frame_init(FrameState& frame)
    {
        frame = {};
        game_init(frame.game);
        frame.snapshot = game_snapshot(frame.game);
        frame.pending.aim = frame.game.aim;
    }

    void frame_advance(FrameState& frame, double elapsed, const Input& input)
    {
        if (!input_push(frame.pending, input))
        {
            frame.input_overflow = true;
        }
        if (elapsed < 0)
        {
            elapsed = 0;
        }
        frame.accumulator += elapsed;
        if (frame.accumulator > 0.25)
        {
            frame.accumulator = 0.25;
        }
        frame.ticks_this_frame = 0;
        constexpr double STEP = 1.0 / 60.0;
        while (frame.accumulator >= STEP)
        {
            game_tick(frame.game, input_pop(frame.pending));
            frame.accumulator -= STEP;
            frame.ticks_this_frame++;
        }
        if (frame.ticks_this_frame > 0)
        {
            frame.snapshot = game_snapshot(frame.game);
        }
    }
}