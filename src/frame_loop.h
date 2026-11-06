//
// Created by t.laurent on 02/11/2026.
//
#pragma once
#include "game_types.h"

namespace missile_command {

    constexpr int INPUT_CAPACITY = 16;

    struct InputBuffer {

        Input items[INPUT_CAPACITY];
        int head;
        int count;
        Vec2 aim;
    };

    struct FrameState {

        GameState game;
        Snapshot snapshot;
        InputBuffer pending;
        double accumulator;
        int ticks_this_frame;
        bool input_overflow;

    };

    bool input_push(InputBuffer& buffer,Input& input);
    Input input_pop(InputBuffer& buffer);
    void frame_init(FrameState& frame);
    void frame_advance(FrameState& frame,double elapsed,const Input& input);

}