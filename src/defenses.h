//
// Created by t.laurent on 02/11/2026.
//
#pragma once
#include "game_types.h"

namespace missile_command {

    void defenses_new_wave(Defenses& defenses);
    void defenses_reset(Defenses& defenses);
    int defenses_living_cities(const Defenses& defenses);
    int defenses_ammo(const Defenses &defenses);
    int defenses_restore_cities(Defenses &defenses,int available);

    void defenses_tick(Defenses& defenses,const Explosions& explosions,const Input& input,int free_shots,FireOrders& orders);


}
