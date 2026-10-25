#ifndef PX_BATTLE_UI_H
#define PX_BATTLE_UI_H
#include <stdint.h>

#include "battle.h"

/* The viewer's battle screen and menu (self-authored, not the game's battle
 * scene or UI art; Pokémon without a loaded battle model are drawn as
 * type-coloured placeholders). Phases: messages (paged with confirm),
 * the command menu (FIGHT / BALL / RUN), the move menu (four moves; cancel goes
 * back), then the turn's messages; the battle ends after its last page. */
enum px_battle_phase { PX_UI_MESSAGES = 0, PX_UI_COMMAND, PX_UI_MOVES, PX_UI_DONE };
enum { PX_UI_MAX_PAGES = 24 };
struct px_battle_ui {
    enum px_battle_phase phase;
    int cursor;
    char pages[PX_UI_MAX_PAGES][96];
    uint8_t page_event[PX_UI_MAX_PAGES]; /* the battle log's PX_LOG_* of each page (0 otherwise) */
    uint32_t page_count, page;
    const char *const *species_names, *const *move_names, *const *type_names;
    uint32_t species_count, move_count;
};
/* Start: "A wild X appeared!" and "Go! Y!". */
void px_battle_ui_start(struct px_battle_ui *ui, const struct px_battle *battle, const char *const *species_names,
                        uint32_t species_count, const char *const *move_names, uint32_t move_count,
                        const char *const *type_names);
/* Input: 'U', 'D', 'L', 'R' move the cursor, '.' confirms, 'x' cancels.
 * Returns 1 when the input ran a battle turn. */
int px_battle_ui_input(struct px_battle_ui *ui, struct px_battle *battle, char key);
void px_battle_ui_draw(uint8_t *rgba, uint32_t width, uint32_t height, const struct px_battle_ui *ui,
                       const struct px_battle *battle);
/* The same in two passes, so that battle models can be drawn between them:
 * the stage (sky, ground, platforms, and the placeholders of the sides whose
 * bit in modeled is clear; bit 0 the player's side, bit 1 the wild one), then
 * the HUD (information boxes, menus and messages). The platforms' centres:
 * the wild side at (w * 3/4, h * 9/20), the player's at (w / 4, h * 3/4 - 8). */
void px_battle_ui_draw_stage(uint8_t *rgba, uint32_t width, uint32_t height, const struct px_battle *battle,
                             unsigned modeled);
void px_battle_ui_draw_hud(uint8_t *rgba, uint32_t width, uint32_t height, const struct px_battle_ui *ui,
                           const struct px_battle *battle);
#endif
