//
// Created by t.laurent on 02/11/2026.
//
#pragma once

#include "game_types.h"

namespace missile_command {

    void missiles_reset(Missiles& missiles,std::uint32_t& seed);
    int missiles_free_shots(const Missiles& missiles);

    bool missiles_empty(const Missiles& missiles);

    bool missiles_spawn_threat(Missiles& missiles,Vec2 from, Vec2 to,EnemyKind kind,float speed);
    void missiles_tick(Missiles& missiles,
        const Defenses& defenses,
        const Explosions& explosions,
        const FireOrders& fire,
        const SpawnOrders& spawns,
        MissileEvents& events);
}
