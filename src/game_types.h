//
// Created by t.laurent on 02/11/2026.
//
#pragma once
#include <cstdint>

namespace missile_command {
    constexpr int SCREEN_WIDTH = 960;
    constexpr int SCREEN_HEIGHT = 720;
    constexpr float GROUND_Y = 580.0f;
    constexpr float TICK_SECONDS = 1.0f / 60.0f;
    constexpr int CITY_COUNT = 6;
    constexpr int BATTERY_COUNT = 3;
    constexpr int MAX_SHOTS = 64;
    constexpr int MAX_BLASTS = 256;
    constexpr int MAX_EVENTS = 128;
    constexpr int AMMO_PER_BATTERY = 10;
    constexpr int MAX_THREATS = 128;

    constexpr int MAX_AIRCRAFT = 4;

    struct Vec2 {
        float x, y;
    };

    enum class Phase {
        title,
        playing,
        wave_bonus,
        paused,
        game_over
    };

    enum class EnemyKind {
        ballistic,
        splitter,
        smart_bomb
    };

    enum class AicraftKind {
        bomber,
        satellite
    };

    struct Input {

        Vec2 aim;
        bool fire[BATTERY_COUNT];
        bool start;
        bool pause;
    };

    struct City {
        Vec2 position;
        bool alive;
    };

    struct Battery {
        Vec2 position;
        int ammo;
        bool alive;
    };


    struct Defenses {
        City cities[CITY_COUNT];
        Battery batteries[BATTERY_COUNT];
    };

    struct Blast {
        Vec2 position;
        float age;
        float radius;
        bool dangerous;
        bool active;
    };

    struct Explosions {
        Blast items[MAX_BLASTS];
    };

    struct  BlastRequest {
        Vec2 position;
        bool dangerous;
    };

    struct MissileEvents {
        BlastRequest blasts[MAX_EVENTS];
        int blast_count;
        int base_points;
        bool capacity_reached;
    };

    struct Shot {
        Vec2 origin;
        Vec2 position;
        Vec2 target;
        Vec2 velocity;
        bool active;
    };

    struct Threat {
        Vec2 origin;
        Vec2 position;
        Vec2 target;
        Vec2 velocity;
        float speed;
        EnemyKind kind;
        bool active;
    };

    struct Aircraft {
        Vec2 position;
        float direction;
        float drop_timer;
        int drops_left;
        AicraftKind kind;
        bool active;
    };

    struct Missiles {

        Shot shots[MAX_SHOTS];
        Threat threats[MAX_THREATS];
        Aircraft aircraft[MAX_AIRCRAFT];
        std::uint32_t random_state;
    };

    struct FireOrder {
        Vec2 origin;
        Vec2 target;
        float speed;
    };

    struct FireOrders {
        FireOrder items[BATTERY_COUNT];
        int count;
    };

    struct SpawnOrders {
        bool missile;
        EnemyKind kind;
        float speed;
        bool aircraft;
        AicraftKind aicraft_kind;
    };

    struct Wave {

        int number;
        int remaining;
        int launched;
        float spawn_timer;
        bool aircraft_sent;
        float bonus_timer;
        int bonus_points;
    };

    struct GameState {
        Phase phase;
        Defenses defenses;
        Missiles missiles;
        Explosions explosions;


        float spawn_timer;
        Wave wave;
        int score;
        int bonus_cities;
        int bonus_next_score;

        std::uint32_t seed;

        int ticks;
        bool capacity_warning;
        Vec2 aim;
    };

    struct Snapshot {
        GameState game;
    };

}
