//
// Created by t.laurent on 03/11/2026.
//
#pragma once
#include "game_types.h"

namespace missile_command
{
    void explosions_reset(Explosions& explosions);
    void explosions_advance(Explosions& explosions);
    bool explosions_add(Explosions& explosions, const MissileEvents& events);
    bool explosions_empty(const Explosions& explosions);}
