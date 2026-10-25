#include <SDL.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SCREEN_W 960
#define SCREEN_H 540
#define WORLD_W 24
#define WORLD_H 16
#define TILE 48
#define SAVE_FILE "pokemon-slice.save"

struct game {
	float x;
	float y;
	float camera_x;
	float camera_y;
	int hp;
	int enemy_hp;
	int wins;
	int mode;
	int dialogue;
	bool running;
};

static const char *const glyphs[37] = {
	"01110 10001 10011 10101 11001 10001 01110", /* A */
	"11110 10001 10001 11110 10001 10001 11110", /* B */
	"01111 10000 10000 10000 10000 10000 01111", /* C */
	"11110 10001 10001 10001 10001 10001 11110", /* D */
	"11111 10000 10000 11110 10000 10000 11111", /* E */
	"11111 10000 10000 11110 10000 10000 10000", /* F */
	"01111 10000 10000 10111 10001 10001 01111", /* G */
	"10001 10001 10001 11111 10001 10001 10001", /* H */
	"11111 00100 00100 00100 00100 00100 11111", /* I */
	"00111 00010 00010 00010 10010 10010 01100", /* J */
	"10001 10010 10100 11000 10100 10010 10001", /* K */
	"10000 10000 10000 10000 10000 10000 11111", /* L */
	"10001 11011 10101 10101 10001 10001 10001", /* M */
	"10001 11001 10101 10011 10001 10001 10001", /* N */
	"01110 10001 10001 10001 10001 10001 01110", /* O */
	"11110 10001 10001 11110 10000 10000 10000", /* P */
	"01110 10001 10001 10001 10101 10010 01101", /* Q */
	"11110 10001 10001 11110 10100 10010 10001", /* R */
	"01111 10000 10000 01110 00001 00001 11110", /* S */
	"11111 00100 00100 00100 00100 00100 00100", /* T */
	"10001 10001 10001 10001 10001 10001 01110", /* U */
	"10001 10001 10001 10001 10001 01010 00100", /* V */
	"10001 10001 10001 10101 10101 11011 10001", /* W */
	"10001 10001 01010 00100 01010 10001 10001", /* X */
	"10001 10001 01010 00100 00100 00100 00100", /* Y */
	"11111 00001 00010 00100 01000 10000 11111", /* Z */
	"00000 00100 00000 00000 00100 00000 00000", /* ! */
	"00000 00000 00000 00000 00000 00000 00100", /* . */
	"00000 00000 00000 00000 00000 00000 00000", /* space */
	"01110 10001 10011 10101 11001 10001 01110", /* 0 approximated */
	"00100 01100 00100 00100 00100 00100 01110", /* 1 */
	"01110 10001 00001 00010 00100 01000 11111", /* 2 */
	"11110 00001 00001 01110 00001 00001 11110", /* 3 */
	"00010 00110 01010 10010 11111 00010 00010", /* 4 */
	"11111 10000 10000 11110 00001 00001 11110", /* 5 */
	"00110 01000 10000 11110 10001 10001 01110", /* 6 */
	"11111 00001 00010 00100 01000 01000 01000"  /* 7 */
};

static int glyph_index(char c)
{
	if (c >= 'A' && c <= 'Z') return c - 'A';
	if (c == '!') return 26;
	if (c == '.') return 27;
	if (c == ' ') return 28;
	if (c >= '0' && c <= '7') return 29 + c - '0';
	return 28;
}

static void text(SDL_Renderer *r, int x, int y, int scale, const char *value, SDL_Color color)
{
	SDL_SetRenderDrawColor(r, color.r, color.g, color.b, color.a);
	for (; *value != '\0'; value++, x += 6 * scale) {
		const char *pattern = glyphs[glyph_index(*value)];
		for (int row = 0; row < 7; row++) {
			for (int col = 0; col < 5; col++) {
				if (pattern[row * 6 + col] == '1') {
					SDL_Rect pixel = {x + col * scale, y + row * scale, scale, scale};
					SDL_RenderFillRect(r, &pixel);
				}
			}
		}
	}
}

static void save_game(const struct game *g)
{
	FILE *file = fopen(SAVE_FILE, "wb");
	if (file == NULL) return;
	(void)fwrite(g, sizeof *g, 1, file);
	(void)fclose(file);
}

static void load_game(struct game *g)
{
	FILE *file = fopen(SAVE_FILE, "rb");
	if (file != NULL) {
		if (fread(g, sizeof *g, 1, file) != 1) {
			(void)fclose(file);
			return;
		}
		(void)fclose(file);
	}
}

static void draw_world(SDL_Renderer *r, const struct game *g)
{
	SDL_SetRenderDrawColor(r, 30, 90, 72, 255);
	SDL_RenderClear(r);
	for (int row = 0; row < WORLD_H; row++) {
		for (int col = 0; col < WORLD_W; col++) {
			SDL_Rect tile = {(int)(col * TILE - g->camera_x), (int)(row * TILE - g->camera_y), TILE - 2, TILE - 2};
			bool path = row == 7 || col == 11 || (row > 4 && row < 10 && col > 3 && col < 8);
			SDL_SetRenderDrawColor(r, path ? 174 : 58, path ? 136 : 145, path ? 75 : 75, 255);
			SDL_RenderFillRect(r, &tile);
		}
	}
	SDL_Rect npc = {(int)(15 * TILE - g->camera_x), (int)(7 * TILE - g->camera_y), 28, 36};
	SDL_SetRenderDrawColor(r, 224, 92, 133, 255);
	SDL_RenderFillRect(r, &npc);
	SDL_Rect player = {(int)(g->x - g->camera_x), (int)(g->y - g->camera_y), 28, 36};
	SDL_SetRenderDrawColor(r, 70, 130, 245, 255);
	SDL_RenderFillRect(r, &player);
}

static void draw_bar(SDL_Renderer *r, int x, int y, int value, int max, SDL_Color color)
{
	SDL_Rect outline = {x, y, 204, 20};
	SDL_SetRenderDrawColor(r, 15, 20, 25, 255);
	SDL_RenderFillRect(r, &outline);
	SDL_Rect bar = {x + 2, y + 2, (200 * value) / max, 16};
	SDL_SetRenderDrawColor(r, color.r, color.g, color.b, color.a);
	SDL_RenderFillRect(r, &bar);
}

static void draw_battle(SDL_Renderer *r, const struct game *g)
{
	SDL_SetRenderDrawColor(r, 35, 42, 75, 255);
	SDL_RenderClear(r);
	SDL_SetRenderDrawColor(r, 100, 150, 85, 255);
	SDL_Rect field = {0, 330, SCREEN_W, 210};
	SDL_RenderFillRect(r, &field);
	SDL_SetRenderDrawColor(r, 220, 90, 90, 255);
	SDL_Rect enemy = {650, 190, 90, 90};
	SDL_RenderFillRect(r, &enemy);
	SDL_SetRenderDrawColor(r, 75, 125, 235, 255);
	SDL_Rect hero = {210, 300, 100, 100};
	SDL_RenderFillRect(r, &hero);
	SDL_Color white = {245, 245, 240, 255};
	text(r, 650, 150, 3, "WILD MON", white);
	text(r, 210, 260, 3, "PLAYER", white);
	draw_bar(r, 650, 250, g->enemy_hp, 30, (SDL_Color){235, 190, 50, 255});
	draw_bar(r, 210, 410, g->hp, 30, (SDL_Color){70, 220, 110, 255});
	SDL_SetRenderDrawColor(r, 18, 24, 42, 255);
	SDL_Rect box = {80, 455, 800, 70};
	SDL_RenderFillRect(r, &box);
	text(r, 105, 475, 3, "SPACE ATTACK   ESC RUN", white);
}

int main(int argc, char **argv)
{
	bool demo = argc > 1 && strcmp(argv[1], "--demo") == 0;
	int frame_limit = 0;
	for (int i = 2; i + 1 < argc; i++) {
		if (strcmp(argv[i], "--frames") == 0) frame_limit = atoi(argv[i + 1]);
	}
	if (demo && frame_limit == 0) frame_limit = 3;
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) return 1;
	SDL_Window *window = SDL_CreateWindow("Pokemon X - Playable Slice", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_W, SCREEN_H, 0);
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, demo ? SDL_RENDERER_SOFTWARE : SDL_RENDERER_ACCELERATED);
	if (window == NULL || renderer == NULL) return 1;
	struct game game = {11 * TILE, 7 * TILE, 0, 0, 30, 30, 0, 0, 0, true};
	bool key_was_down = false;
	Uint32 start = SDL_GetTicks();
	while (game.running) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) game.running = false;
			if (event.type == SDL_KEYDOWN && !event.key.repeat) {
				SDL_Keycode key = event.key.keysym.sym;
				if (key == SDLK_ESCAPE) {
					if (game.mode == 1) game.mode = 0;
					else game.running = false;
				}
				if (key == SDLK_SPACE && game.mode == 1) game.enemy_hp -= 10;
				if (key == SDLK_SPACE && game.mode == 0 && game.x > 13 * TILE && game.x < 17 * TILE) game.dialogue = 1;
				if ((key == SDLK_e || key == SDLK_SPACE) && game.mode == 0 && game.x > 13 * TILE && game.x < 17 * TILE) game.dialogue = 1;
				if (key == SDLK_b && game.mode == 0) game.mode = 1;
				if (key == SDLK_F5) save_game(&game);
				if (key == SDLK_F9) load_game(&game);
			}
		}
		const Uint8 *keys = SDL_GetKeyboardState(NULL);
		if (game.mode == 0 && game.dialogue == 0) {
			float speed = keys[SDL_SCANCODE_LSHIFT] ? 4.0f : 2.5f;
			if (keys[SDL_SCANCODE_W]) game.y -= speed;
			if (keys[SDL_SCANCODE_S]) game.y += speed;
			if (keys[SDL_SCANCODE_A]) game.x -= speed;
			if (keys[SDL_SCANCODE_D]) game.x += speed;
			if (keys[SDL_SCANCODE_LEFT]) game.camera_x -= 2.0f;
			if (keys[SDL_SCANCODE_RIGHT]) game.camera_x += 2.0f;
			if (keys[SDL_SCANCODE_UP]) game.camera_y -= 2.0f;
			if (keys[SDL_SCANCODE_DOWN]) game.camera_y += 2.0f;
			if (game.x > 14 * TILE && game.x < 16 * TILE && game.y > 6 * TILE && game.y < 9 * TILE) game.dialogue = 1;
		}
		if (game.enemy_hp <= 0) {
			game.wins++;
			game.enemy_hp = 30;
			game.mode = 0;
		}
		game.camera_x += (game.x - SCREEN_W / 2.0f - game.camera_x) * 0.05f;
		game.camera_y += (game.y - SCREEN_H / 2.0f - game.camera_y) * 0.05f;
		if (game.camera_x < 0) game.camera_x = 0;
		if (game.camera_y < 0) game.camera_y = 0;
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
		if (game.mode == 1) draw_battle(renderer, &game);
		else {
			draw_world(renderer, &game);
			SDL_Color white = {245, 245, 240, 255};
			text(renderer, 20, 20, 3, "WASD MOVE  B BATTLE  F5 SAVE  F9 LOAD", white);
			if (game.dialogue != 0) {
				SDL_SetRenderDrawColor(renderer, 18, 24, 42, 245);
				SDL_Rect box = {70, 390, 820, 110};
				SDL_RenderFillRect(renderer, &box);
				text(renderer, 100, 415, 3, "HELLO TRAINER!", white);
				text(renderer, 100, 450, 3, "PRESS SPACE TO CLOSE", white);
				if (!key_was_down && keys[SDL_SCANCODE_SPACE]) game.dialogue = 0;
			}
		}
		SDL_RenderPresent(renderer);
		key_was_down = keys[SDL_SCANCODE_SPACE] != 0;
		if (frame_limit > 0 && (int)((SDL_GetTicks() - start) / 16) >= frame_limit) game.running = false;
		SDL_Delay(16);
	}
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
