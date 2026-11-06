//
// Created by t.laurent on 02/11/2026.
//

#pragma once
#include "game_types.h"
#include <cstdint>

namespace missile_command {

    void game_init(GameState& game,uint32_t seed = 1984);
    void game_tick(GameState& game,const Input& input);
    Snapshot game_snapshot(const GameState& game);
}
