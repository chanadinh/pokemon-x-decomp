#ifndef PX_BATTLE_UI_H
#define PX_BATTLE_UI_H
#include <stdint.h>

#include "battle.h"

/* The viewer's battle screen and menu (self-authored, not the game's battle
 * scene or UI art; Pokémon are drawn as type-coloured placeholders until
 * their battle models are loaded). Phases: messages (paged with confirm),
 * the command menu (FIGHT / RUN), the move menu (four moves; cancel goes
 * back), then the turn's messages; the battle ends after its last page. */
enum px_battle_phase { PX_UI_MESSAGES = 0, PX_UI_COMMAND, PX_UI_MOVES, PX_UI_DONE };
enum { PX_UI_MAX_PAGES = 24 };
struct px_battle_ui {
    enum px_battle_phase phase;
    int cursor;
    char pages[PX_UI_MAX_PAGES][96];
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
#endif
