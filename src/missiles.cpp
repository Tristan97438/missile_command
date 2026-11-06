//
// Created by t.laurent on 02/11/2026.
//

#include "missiles.h"
#include "math2d.h"

namespace missile_command {

    static std::uint32_t next_random(std::uint32_t& state)
    {
        state ^= state << 13;
        state ^= state >> 17;
        state ^= state << 5;
        return state;
    }

    static Vec2 choose_target(const Defenses& defenses, std::uint32_t& random)
    {
        Vec2 targets[CITY_COUNT + BATTERY_COUNT];
        int count = 0;
        for (const City& city : defenses.cities)
        {
            if (city.alive)
            {
                targets[count++] = {city.position.x, GROUND_Y};
            }
        }
        for (const Battery& battery : defenses.batteries)
        {
            if (battery.alive)
            {
                targets[count++] = {battery.position.x, GROUND_Y};
            }
        }
        if (count == 0)
        {
            return {SCREEN_WIDTH * 0.5f, GROUND_Y};
        }
        return targets[next_random(random) % static_cast<unsigned int>(count)];
    }

    void missiles_reset(Missiles &missiles,std::uint32_t& seed) {
        missiles = {};
        missiles.random_state = seed == 0 ? 1 : seed;
    }

    int missiles_free_shots(const Missiles &missiles) {
        int count = 0;
        for (const Shot& shot : missiles.shots)
        {
            if (!shot.active)
            {
                count++;
            }
        }
        return count;
    }
    bool missiles_empty(const Missiles& missiles)
    {
        for (const Shot& shot : missiles.shots)
        {
            if (shot.active)
            {
                return false;
            }
        }
        for (const Threat& threat : missiles.threats)
        {
            if (threat.active)
            {
                return false;
            }
        }
        for (const Aircraft& aircraft : missiles.aircraft)
        {
            if (aircraft.active) return false;

        }
        return true;
    }

    bool missiles_spawn_threat(Missiles& missiles,Vec2 from, Vec2 to,EnemyKind kind,float speed) {

        for (Threat& threat : missiles.threats)
        {
            if (threat.active)
            {
                continue;
            }
            threat = {from, from, to, velocity_toward(from, to, speed),speed,kind, true};
            return true;
        }
        return false;
    };

    static void add_blast(MissileEvents &events, Vec2 position,bool dangerous) {

        if (events.blast_count == MAX_EVENTS) {
            events.capacity_reached = true;
            return;
        }
        events.blasts[events.blast_count++] = {position,dangerous};
    }

    static bool touches_blast(
        Vec2 before, Vec2 after, const Explosions& explosions, float extra_radius)
    {
        for (const Blast& blast : explosions.items)
        {
            if (blast.active &&
                segment_hits_circle(before, after, blast.position, blast.radius + extra_radius))
            {
                return true;
            }
        }
        return false;
    }

    static void fire_shots(Missiles &missiles,const FireOrders& fire,MissileEvents& events) {
        for (int i = 0; i < fire.count; i++)
        {
            const FireOrder& order = fire.items[i];
            bool placed = false;

            for (Shot& shot : missiles.shots)
            {
                if (shot.active)
                {
                    continue;
                }
                shot = {order.origin,
                    order.origin,
                    order.target,
                    velocity_toward(order.origin, order.target, order.speed),
                    true};
                placed = true;
                break;
            }

            if (!placed) {
                events.capacity_reached = true;
            }
        }
    }

    static void advance_shots(Missiles &missiles,MissileEvents& events) {

        for (Shot& shot : missiles.shots)
        {
            if (!shot.active)
            {

                continue;
            }
            Vec2 next = {shot.position.x + shot.velocity.x * TICK_SECONDS,
                shot.position.y + shot.velocity.y * TICK_SECONDS};
            if (reached_target(shot.position, next, shot.target))
            {
                shot.position = shot.target;
                shot.active = false;
                add_blast(events,shot.position,false);
            }
            else
            {
                shot.position = next;
            }
        }
    }

    static void launch_aircraft(Missiles& missiles,AicraftKind kind, MissileEvents& events) {

        for (Aircraft& aircraft : missiles.aircraft) {
            if (aircraft.active) continue;

            float direction = next_random(missiles.random_state) % 2 == 0? 1.0f : -1.0f;
            float y = 105.0f + static_cast<float>(next_random(missiles.random_state)%70);
            aircraft = {
                {direction > 0 ? -20.0f : SCREEN_WIDTH +20.0f,y},
                direction, 0.9f,3,kind,true
            };
            return;
        }
        events.capacity_reached = true;
    }

    static void advance_aircraft(Missiles &missiles,
        const Defenses& defenses,
        const Explosions& explosions,
        float threat_speed,
        MissileEvents& events) {

        for (Aircraft& aircraft : missiles.aircraft)
        {
            if (!aircraft.active)
            {
                continue;
            }
            float speed = aircraft.kind == AicraftKind::satellite ? 165.0f : 105.0f;
            Vec2 next = {aircraft.position.x + aircraft.direction * speed * TICK_SECONDS,
                aircraft.position.y};
            if (touches_blast(aircraft.position, next, explosions, 10))
            {
                aircraft.active = false;
                events.base_points += 100;
                add_blast(events, aircraft.position, false);
                continue;
            }
            aircraft.position = next;
            if (next.x < -25 || next.x > SCREEN_WIDTH + 25)
            {
                aircraft.active = false;
                continue;
            }
            aircraft.drop_timer -= TICK_SECONDS;
            if (aircraft.drop_timer <= 0 && aircraft.drops_left > 0)
            {
                Vec2 target = choose_target(defenses, missiles.random_state);
                if (!missiles_spawn_threat(
                        missiles, next, target, EnemyKind::ballistic, threat_speed))
                {
                    events.capacity_reached = true;
                }
                aircraft.drops_left--;
                aircraft.drop_timer = 1.2f;
            }
        }
    }

    static void advanced_threats(Missiles &missiles,
        const Defenses& defenses,
        const Explosions& explosions,
        MissileEvents& events) {

        struct Child {
            Vec2 position;
            float speed;
        };

        Child children[MAX_THREATS];
        int child_count = 0;

        for (Threat& threat : missiles.threats) {

            if (!threat.active) {
                continue;
            }

            Vec2 next = {threat.position.x +threat.velocity.x * TICK_SECONDS,
                         threat.position.y +threat.velocity.y * TICK_SECONDS};

            if (touches_blast(threat.position, next, explosions, 1.0f)) {

                threat.active = false;
                events.base_points += 25;
                add_blast(events,next,false);
                continue;
            }

            if (next.y > GROUND_Y || reached_target(threat.position, next, threat.target)) {

                threat.active = false;
                add_blast(events, {next.x, GROUND_Y}, true);
                continue;
            }
            threat.position = next;
            if (threat.kind == EnemyKind::splitter && next.y > 220) {

                threat.active = false;

                for (int branch = 0; branch < 3; branch++) {

                    if (child_count < MAX_THREATS) {
                        children[child_count ++] = {next,threat.speed};
                    }
                    else {
                        events.capacity_reached = true;
                    }
                }


            }

        }

        for (int i = 0; i < child_count; i++) {

            if (!missiles_spawn_threat(missiles,
            children[i].position,
              choose_target(defenses, missiles.random_state),
                 EnemyKind::ballistic,
                 children[i].speed))
            {
                events.capacity_reached = true;
            }
        }
    }

    void missiles_tick(Missiles &missiles,
        const Defenses& defenses,
        const Explosions& explosions,
        const FireOrders& fire,
        const SpawnOrders& spawn,
        MissileEvents& events) {

        events = {};
        fire_shots(missiles,fire,events);
        advance_shots(missiles,events);

        if (spawn.missile) {

            Vec2 origin = {30.0f + static_cast<float>(next_random(missiles.random_state) %900),75};

            if (!missiles_spawn_threat(missiles,
                origin,
                choose_target(defenses,
                    missiles.random_state),
                    spawn.kind,
                    spawn.speed)) {

                events.capacity_reached = true;

            }
        }
        if (spawn.aircraft) {
            launch_aircraft(missiles,spawn.aicraft_kind,events);
        }
        advance_aircraft(missiles,defenses,explosions,spawn.speed,events);
        advanced_threats(missiles,defenses,explosions,events);
    }
}
