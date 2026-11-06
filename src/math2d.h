//
// Created by t.laurent on 02/11/2026.
//
#pragma once
#include "game_types.h"

namespace missile_command {
    float clamp(float val, float low, float high);
    Vec2 velocity_toward(Vec2 from, Vec2 to, float speed);
    bool reached_target(Vec2 before, Vec2 after, Vec2 target);

    float distance_squared(Vec2 a, Vec2 b);
    bool segment_hits_circle(Vec2 from, Vec2 to, Vec2 center, float radius);

    bool circle_hits_building(Vec2 center,float radius,Vec2 building,float half_width,float half_height);

}