#include "renderer.h"
#include "raylib.h"
#include <cstdio>

namespace missile_command
{
	static Vector2 visual(Vec2 point)
	{
		return {point.x, point.y};
	}

	static void centered_text(const char* text, int y, int size, Color color)
	{
		DrawText(text, (SCREEN_WIDTH - MeasureText(text, size)) / 2, y, size, color);
	}

	static void draw_city(const City& city)
	{
		int x = static_cast<int>(city.position.x);
		int y = static_cast<int>(GROUND_Y);
		if (!city.alive)
		{
			DrawLine(x - 20, y - 1, x - 10, y + 5, DARKBROWN);
			DrawLine(x - 10, y + 5, x + 12, y + 5, DARKBROWN);
			DrawLine(x + 12, y + 5, x + 20, y - 1, DARKBROWN);
			return;
		}
		DrawRectangle(x - 22, y - 17, 44, 17, BLUE);
		DrawRectangle(x - 16, y - 26, 10, 26, SKYBLUE);
		DrawRectangle(x, y - 33, 9, 33, BLUE);
		DrawRectangle(x + 12, y - 24, 8, 24, SKYBLUE);
		for (int row = 0; row < 3; row++)
		{
			DrawRectangle(x - 14, y - 22 + row * 7, 3, 3, BLACK);
			DrawRectangle(x + 2, y - 29 + row * 8, 3, 3, BLACK);
		}
	}

	static void draw_battery(const Battery& battery, int index)
	{
		constexpr const char* LABELS[BATTERY_COUNT] = {"ALPHA [Q]", "DELTA [S]", "OMEGA [D]"};
		int x = static_cast<int>(battery.position.x);
		int y = static_cast<int>(GROUND_Y);
		Color mound = {230, 203, 57, 255};
		DrawTriangle({static_cast<float>(x - 50), static_cast<float>(y + 1)},
			{static_cast<float>(x + 50), static_cast<float>(y + 1)},
			{static_cast<float>(x), static_cast<float>(y - 34)},
			mound);
		if (battery.alive)
		{
			DrawRectangle(x - 6, y - 28, 12, 13, SKYBLUE);
			DrawLine(x, y - 35, x, y - 27, SKYBLUE);
		}
		else
		{
			DrawLine(x - 18, y - 5, x + 18, y - 5, DARKBROWN);
		}
		const char* label = LABELS[index];
		DrawText(label, x - MeasureText(label, 16) / 2, y + 38, 16, SKYBLUE);
		for (int i = 0; i < battery.ammo; i++)
		{
			int ammo_x = x - 24 + (i % 5) * 11;
			int ammo_y = y + 10 + (i / 5) * 12;
			DrawRectangle(ammo_x, ammo_y, 3, 8, WHITE);
			DrawPixel(ammo_x + 1, ammo_y - 1, WHITE);
		}
		if (battery.ammo == 0)
		{
			const char* state = battery.alive ? "OUT" : "DETRUITE";
			DrawText(state, x - MeasureText(state, 18) / 2, y + 12, 18, RED);
		}
	}

	static void draw_missiles(const Missiles& missiles)
	{
		for (const Shot& shot : missiles.shots)
		{
			if (!shot.active)
			{
				continue;
			}
			DrawLineV(visual(shot.origin), visual(shot.position), SKYBLUE);
			DrawCircleV(visual(shot.position), 2, WHITE);
			DrawLine(static_cast<int>(shot.target.x) - 3,
				static_cast<int>(shot.target.y),
				static_cast<int>(shot.target.x) + 3,
				static_cast<int>(shot.target.y),
				DARKGRAY);
		}
		for (const Threat& threat : missiles.threats)
		{
			if (!threat.active)
			{
				continue;
			}
			if (threat.kind == EnemyKind::smart_bomb)
			{
				int x = static_cast<int>(threat.position.x);
				int y = static_cast<int>(threat.position.y);
				DrawLine(x, y - 7, x + 5, y, WHITE);
				DrawLine(x + 5, y, x, y + 7, WHITE);
				DrawLine(x, y + 7, x - 5, y, WHITE);
				DrawLine(x - 5, y, x, y - 7, WHITE);
			}
			else
			{
				DrawLineV(visual(threat.origin), visual(threat.position), RED);
				DrawCircleV(visual(threat.position), 2, YELLOW);
			}
		}
		for (const Aircraft& aircraft : missiles.aircraft)
		{
			if (!aircraft.active)
			{
				continue;
			}
			int x = static_cast<int>(aircraft.position.x);
			int y = static_cast<int>(aircraft.position.y);
			if (aircraft.kind == AicraftKind::bomber)
			{
				DrawLine(x - 14, y, x + 14, y, SKYBLUE);
				DrawLine(x, y - 7, x, y + 7, SKYBLUE);
				DrawLine(x - 9, y - 4, x + 9, y + 4, SKYBLUE);
			}
			else
			{
				DrawRectangleLines(x - 7, y - 5, 14, 10, SKYBLUE);
				DrawLine(x - 14, y - 7, x - 14, y + 7, SKYBLUE);
				DrawLine(x + 14, y - 7, x + 14, y + 7, SKYBLUE);
				DrawLine(x - 14, y, x + 14, y, SKYBLUE);
			}
		}
	}

	void render_step(const Snapshot& snapshot, bool input_overflow)
	{
		const GameState& game = snapshot.game;
		BeginDrawing();
		ClearBackground(BLACK);
		DrawRectangle(0, static_cast<int>(GROUND_Y), SCREEN_WIDTH, 9, {230, 203, 57, 255});
		for (const City& city : game.defenses.cities)
		{
			draw_city(city);
		}
		for (int i = 0; i < BATTERY_COUNT; i++)
		{
			draw_battery(game.defenses.batteries[i], i);
		}
		draw_missiles(game.missiles);
		for (const Blast& blast : game.explosions.items)
		{
			if (!blast.active)
			{
				continue;
			}
			Color color = blast.dangerous ? ORANGE : YELLOW;
			DrawCircleV(visual(blast.position), blast.radius, color);
			DrawCircleLines(static_cast<int>(blast.position.x),
				static_cast<int>(blast.position.y),
				blast.radius,
				WHITE);
			if (blast.radius > 5)
			{
				DrawCircleV(
					visual(blast.position), blast.radius * 0.55f, blast.dangerous ? RED : SKYBLUE);
			}
		}

		char text[128];
		DrawText("MISSILE COMMAND", 24, 18, 22, SKYBLUE);
		std::snprintf(text, sizeof(text), "SCORE %07d", game.score);
		DrawText(text, 355, 18, 24, WHITE);
		std::snprintf(text, sizeof(text), "VAGUE %02d", game.wave.number);
		DrawText(text, 740, 18, 22, YELLOW);
		std::snprintf(text, sizeof(text), "VILLES BONUS %d", game.bonus_cities);
		DrawText(text, 24, 47, 15, GRAY);
		DrawLine(24, 69, SCREEN_WIDTH - 24, 69, DARKGRAY);
		centered_text("SOURIS : VISER    Q / S / D : TIRER    P : PAUSE", 665, 17, LIGHTGRAY);
		centered_text("Chaque batterie dispose de 10 tirs par vague.", 693, 14, GRAY);
		if (game.phase == Phase::playing)
		{
			int x = static_cast<int>(game.aim.x);
			int y = static_cast<int>(game.aim.y);
			DrawCircleLines(x, y, 7, SKYBLUE);
			DrawLine(x - 12, y, x + 12, y, SKYBLUE);
			DrawLine(x, y - 12, x, y + 12, SKYBLUE);
		}
		if (game.phase == Phase::title || game.phase == Phase::paused ||
			game.phase == Phase::game_over)
		{
			DrawRectangle(150, 205, 660, 270, {0, 0, 0, 240});
			DrawRectangleLines(150, 205, 660, 270, SKYBLUE);
			if (game.phase == Phase::title)
			{
				centered_text("DEFENDEZ LES SIX VILLES", 237, 30, WHITE);
				centered_text(
					"Placez vos explosions sur le chemin des missiles.", 293, 19, LIGHTGRAY);
				centered_text(
					"Choisissez une batterie. Economisez ses munitions.", 326, 19, LIGHTGRAY);
				centered_text("[ ENTREE ] COMMENCER", 402, 25, YELLOW);
			}
			else if (game.phase == Phase::paused)
			{
				centered_text("PAUSE", 265, 48, YELLOW);
				centered_text("[ P ] REPRENDRE", 382, 25, WHITE);
			}
			else
			{
				centered_text("THE END", 247, 54, RED);
				std::snprintf(text, sizeof(text), "SCORE FINAL : %d", game.score);
				centered_text(text, 328, 26, WHITE);
				centered_text("[ ENTREE ] NOUVELLE PARTIE", 405, 23, YELLOW);
			}
		}
		if (game.phase == Phase::wave_bonus)
		{
			DrawRectangle(210, 235, 540, 190, {0, 0, 0, 240});
			DrawRectangleLines(210, 235, 540, 190, SKYBLUE);
			centered_text("VAGUE TERMINEE", 263, 30, YELLOW);
			std::snprintf(text, sizeof(text), "VILLES ET MUNITIONS : +%d", game.wave.bonus_points);
			centered_text(text, 323, 23, WHITE);
			centered_text("Rechargement des trois batteries...", 376, 18, SKYBLUE);
		}
		if (game.capacity_warning || input_overflow)
		{
			centered_text("LIMITE DU PROBE ATTEINTE", 86, 18, ORANGE);
		}
		EndDrawing();
	}
}