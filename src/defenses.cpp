//
// Created by t.laurent on 02/11/2026.
//
#include "defenses.h"
#include "math2d.h"
namespace missile_command {

    void defenses_new_wave(Defenses& defenses)
    {
        for (Battery& battery : defenses.batteries)
        {
            battery.ammo = AMMO_PER_BATTERY;
            battery.alive = true;
        }
    }
    void defenses_reset(Defenses &defenses) {

        constexpr float CITY_X[CITY_COUNT] = {170,270,370,590,690,790};
        constexpr float BATTERY_X[BATTERY_COUNT] = {65,480,895};

        for (int i = 0; i < CITY_COUNT; i++) {

            defenses.cities[i] = {{CITY_X[i],GROUND_Y -10},true};
        }

        for (int i = 0; i < BATTERY_COUNT; i++) {

            defenses.batteries[i].position = {BATTERY_X[i],GROUND_Y -20};
        }

        defenses_new_wave(defenses);

    }

    int defenses_restore_cities(Defenses &defenses,int available) {

        int used = 0;
        for ( City& city : defenses.cities) {

            if (!city.alive && used < available) {

                city.alive = true;
                used++;
            }

        }
        return used;
    }
    int defenses_ammo(const Defenses &defenses) {
        int count = 0;
        for (const Battery& battery : defenses.batteries) {
            count += battery.ammo;
        }
        return count;
    }
    int defenses_living_cities(const Defenses& defenses)
    {
        int count = 0;
        for (const City& city : defenses.cities)
        {
            if (city.alive)
            {
                count++;
            }
        }
        return count;
    }
    
    void defenses_tick(Defenses &defenses,const Explosions& explosions, const Input &input, int free_shots, FireOrders &orders) {

        orders ={};

        for (const Blast& blast : explosions.items) {

            if (!blast.active || !blast.dangerous) {
                continue;
            }

            for (City& city : defenses.cities) {

                if (city.alive && circle_hits_building(blast.position,blast.radius,city.position,22,10)) {
                    city.alive = false;
                }
            }

            for (Battery& battery : defenses.batteries) {

                if (battery.alive && circle_hits_building(blast.position,blast.radius,battery.position,25,10)) {
                    battery.alive = false;
                }
            }
        }

        Vec2 aim = {clamp(input.aim.x,0,SCREEN_WIDTH - 8),clamp(input.aim.y,80,GROUND_Y - 42)};

        for (int i = 0; i < BATTERY_COUNT && orders.count < free_shots; i++)
        {
            Battery& battery = defenses.batteries[i];
            if (!input.fire[i] || !battery.alive || battery.ammo == 0)
            {
                continue;
            }
            orders.items[orders.count++] = {battery.position, aim, i ==1 ? 660.0f : 420.0f};
            battery.ammo --;
        }
    }
}
