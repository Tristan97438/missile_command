//
// Created by t.laurent on 03/11/2026.
//
#include "explosions.h"

namespace missile_command {

    void explosions_reset(Explosions &explosions) {

        explosions = {};
    }

    void explosions_advance(Explosions &explosions) {

        for (Blast& blast : explosions.items) {

            if (!blast.active) {

                continue;
            }

            blast.age += TICK_SECONDS;
            constexpr float GROW_TIME = 0.55f;
            constexpr float LIFE_TIME = 1.25f;
            float maximum = blast.dangerous ? 44.0f : 52.0f;

            if (blast.age >= LIFE_TIME) {

                blast.active = false;
                continue;
            }

            if (blast.age < GROW_TIME) {

                blast.radius = maximum * blast.age / GROW_TIME;
            }
            else {

                blast.radius = maximum * (LIFE_TIME - blast.age) / (LIFE_TIME - GROW_TIME);
            }
        }
    }

    bool explosions_add(Explosions &explosions, const MissileEvents &events) {

        bool success = true;
        for (int i = 0; i < events.blast_count; i++) {

            bool placed = false;
            for (Blast& blast : explosions.items) {

                if (blast.active) {
                    continue;
                }

                blast = {events.blasts[i].position,0.0f,2.0f,events.blasts[i].dangerous,true};
                placed = true;
                break;
            }
            if (!placed) {
                success = false;
            }

        }
        return  success;
    }
    bool explosions_empty(const Explosions& explosions)
    {
        for (const Blast& blast : explosions.items)
        {
            if (blast.active)
            {
                return false;
            }
        }
        return true;
    }
}
