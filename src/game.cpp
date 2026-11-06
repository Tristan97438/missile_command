//
// Created by t.laurent on 02/11/2026.
//
#include "game.h"
#include "defenses.h"
#include "explosions.h"
#include "math2d.h"
#include "missiles.h"

namespace missile_command {

    void game_init(GameState& game,std::uint32_t seed ){

        game = {};
        game.phase = Phase::title;
        game.aim = {SCREEN_WIDTH*0.5f,SCREEN_HEIGHT*0.4f};
        game.seed = seed;
        defenses_reset(game.defenses);
        missiles_reset(game.missiles,seed);
        explosions_reset(game.explosions);
    }

    static  SpawnOrders scheduled_wave(GameState& game) {
        SpawnOrders spawn = {};
        spawn.speed = 46.0f;
        if (spawn.speed > 160) {
            spawn.speed = 160;
        }

        game.wave.spawn_timer -= TICK_SECONDS;
        if (game.wave.remaining > 0 && game.wave.spawn_timer <= 0) {

            spawn.missile = true;
            spawn.kind = EnemyKind::ballistic;
            if (game.wave.number >= 3 && game.wave.launched % 5 ==3) {
                spawn.kind = EnemyKind::splitter;
            }

            game.wave.remaining--;
            game.wave.launched++;
            game.wave.spawn_timer = clamp(1.05f - static_cast<float>(game.wave.number) *0.7f,0.28f,1.0f);

        }

        if (game.wave.number >= 2 && game.wave.launched >= 4 && !game.wave.aircraft_sent) {

            spawn.aircraft = true;
            spawn.aicraft_kind = game.wave.number % 2 == 0 ? AicraftKind::bomber : AicraftKind::satellite;
            game.wave.aircraft_sent = true;
        }
        return spawn;
    }
    static void begin_wave(GameState& game) {

        game.wave.number ++;
        game.wave.remaining = 9 + game.wave.number * 3;
        if (game.wave.remaining > 48) {
            game.wave.remaining = 48;
        }

        game.wave.launched = 0;
        game.wave.spawn_timer = 0.7f;
        game.wave.aircraft_sent = false;
        game.wave.bonus_timer = 0;
        defenses_new_wave(game.defenses);
        game.phase = Phase::playing;
    }


    void game_tick(GameState& game,const Input& input) {



        if (game.phase == Phase::title || game.phase == Phase::game_over) {

            if (input.start) {

                std::uint32_t seed = game.seed;
                game_init(game,seed);
                begin_wave(game);

            }
            return;
        }

        if (input.pause && (game.phase == Phase::playing || game.phase == Phase::paused)) {

            game.phase = game.phase == Phase::paused ? Phase::playing : Phase::paused;
            return;
        }

        if (game.phase == Phase::paused) {

            return;
        }
        game.aim = {clamp(input.aim.x,8,SCREEN_WIDTH -8),clamp(input.aim.y,80,SCREEN_HEIGHT - 42)};

        if (game.phase == Phase::wave_bonus) {

            game.wave.bonus_timer += TICK_SECONDS;
            if (game.wave.bonus_timer >= 3.0f) {

                begin_wave(game);
            }
            return;
        }

        explosions_advance(game.explosions);
        FireOrders fire = {};
        defenses_tick(game.defenses,game.explosions,input,missiles_free_shots(game.missiles),fire);

        if (defenses_living_cities(game.defenses) == 0) {

            game.phase = Phase::game_over;
            return;
        }

        SpawnOrders spawn = scheduled_wave(game);
        MissileEvents events = {};
        missiles_tick(game.missiles,game.defenses,game.explosions,fire,spawn,events);


        bool all_blasts_added = explosions_add(game.explosions,events);
        game.capacity_warning = game.capacity_warning ||events.capacity_reached || !all_blasts_added;

        game.score += events.base_points;

        if (game.wave.remaining == 0 && missiles_empty(game.missiles) &&
            explosions_empty(game.explosions))
        {
            game.wave.bonus_timer = 0;
            game.phase = Phase::wave_bonus;
        }
    }

    Snapshot game_snapshot(const GameState &game) {

        return {game};
    }
}