//
// Created by t.laurent on 02/11/2026.
//
#include "math2d.h"
#include <cmath>
namespace missile_command {

    float clamp(float val, float low, float high) {
        if (val < low) return low;
        if (val > high) return high;
        return val;
    }

    Vec2 velocity_toward(Vec2 from, Vec2 to,float speed) {

        float x = to.x - from.x;
        float y = to.y - from.y;
        float length = std::sqrt(x*x + y*y);

        if (length <= 0.0001f) {
            return Vec2(0.f,0.f);
        }

        return Vec2(x/length * speed,y/length * speed);
    }

    bool reached_target(Vec2 before, Vec2 after, Vec2 target)
    {
        return (target.x - before.x) * (target.x - after.x) +
                   (target.y - before.y) * (target.y - after.y) <=
               0.0f;
    }
    float distance_squared(Vec2 a, Vec2 b)
    {
        float x = a.x - b.x;
        float y = a.y - b.y;
        return x * x + y * y;
    }
    bool segment_hits_circle(Vec2 from, Vec2 to, Vec2 center, float radius)
    {
        float x = to.x - from.x;
        float y = to.y - from.y;
        float length_squared = x * x + y * y;
        //float length_squared = distance_squared(to, from);

        float t = 0.0f;
        if (length_squared > 0.0f)
        {
            t = clamp(
                ((center.x - from.x) * x + (center.y - from.y) * y) / length_squared, 0.0f, 1.0f);
        }
        Vec2 closest = {from.x + t * x, from.y + t * y};
        return distance_squared(closest, center) <= radius * radius;
    }
    bool circle_hits_building(
        Vec2 center, float radius, Vec2 building, float half_width, float half_height)
    {
        Vec2 closest = {clamp(center.x, building.x - half_width, building.x + half_width),
            clamp(center.y, building.y - half_height, building.y + half_height)};
        return distance_squared(closest, center) <= radius * radius;
    }


}