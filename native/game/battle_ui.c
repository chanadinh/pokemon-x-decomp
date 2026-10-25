#include "battle_ui.h"

#include <stdio.h>
#include <string.h>

#include "../render/text.h"

static void add_page_event(struct px_battle_ui *ui, const char *text, uint8_t event)
{
    if (ui->page_count >= PX_UI_MAX_PAGES) return;
    ui->page_event[ui->page_count] = event;
    snprintf(ui->pages[ui->page_count++], sizeof ui->pages[0], "%s", text);
}
static void add_page(struct px_battle_ui *ui, const char *text) { add_page_event(ui, text, 0); }

static const char *name_of(const char *const *names, uint32_t count, uint32_t id, char *buffer, size_t size)
{
    if (names && id < count) return names[id];
    snprintf(buffer, size, "#%u", (unsigned)id);
    return buffer;
}

void px_battle_ui_start(struct px_battle_ui *ui, const struct px_battle *b, const char *const *species_names,
                        uint32_t species_count, const char *const *move_names, uint32_t move_count,
                        const char *const *type_names)
{
    memset(ui, 0, sizeof *ui);
    ui->species_names = species_names; ui->species_count = species_count;
    ui->move_names = move_names; ui->move_count = move_count; ui->type_names = type_names;
    char line[96], buffer[16];
    snprintf(line, sizeof line, "A wild %s appeared!",
             name_of(species_names, species_count, b->side[1].species, buffer, sizeof buffer));
    add_page(ui, line);
    snprintf(line, sizeof line, "Go! %s!", name_of(species_names, species_count, b->side[0].species, buffer, sizeof buffer));
    add_page(ui, line);
    ui->phase = PX_UI_MESSAGES;
}

int px_battle_ui_input(struct px_battle_ui *ui, struct px_battle *b, char key)
{
    switch (ui->phase) {
    case PX_UI_MESSAGES:
        if (key != '.') return 0;
        if (++ui->page < ui->page_count) return 0;
        ui->page = ui->page_count = 0;
        ui->phase = b->state == PX_BATTLE_CHOOSING ? PX_UI_COMMAND : PX_UI_DONE;
        ui->cursor = 0;
        return 0;
    case PX_UI_COMMAND:
        if ((key == 'L' || key == 'U') && ui->cursor > 0) --ui->cursor;
        else if ((key == 'R' || key == 'D') && ui->cursor < 2) ++ui->cursor;
        else if (key == '.') {
            if (ui->cursor == 0) { ui->phase = PX_UI_MOVES; ui->cursor = 0; return 0; }
            if (ui->cursor == 1 && !b->balls) {
                add_page(ui, "You have no Poké Balls left!");
                ui->phase = PX_UI_MESSAGES;
                return 0;
            }
            px_battle_turn(b, ui->cursor == 1 ? -2 : -1, ui->species_names, ui->species_count, ui->move_names,
                           ui->move_count);
            for (uint32_t i = 0; i < b->log_count; ++i) add_page_event(ui, b->log[i], b->log_event[i]);
            ui->phase = PX_UI_MESSAGES;
            return 1;
        }
        return 0;
    case PX_UI_MOVES:
        if (key == 'x') { ui->phase = PX_UI_COMMAND; ui->cursor = 0; return 0; }
        if (key == 'L' && ui->cursor % 2) --ui->cursor;
        else if (key == 'R' && !(ui->cursor % 2)) ++ui->cursor;
        else if (key == 'U' && ui->cursor >= 2) ui->cursor -= 2;
        else if (key == 'D' && ui->cursor < 2) ui->cursor += 2;
        else if (key == '.') {
            if (!px_battle_turn(b, ui->cursor, ui->species_names, ui->species_count, ui->move_names, ui->move_count))
                return 0; /* no PP or no move in that slot */
            for (uint32_t i = 0; i < b->log_count; ++i) add_page_event(ui, b->log[i], b->log_event[i]);
            ui->phase = PX_UI_MESSAGES;
            return 1;
        }
        return 0;
    case PX_UI_DONE:
        return 0;
    }
    return 0;
}

/* Type colours for the placeholders (Normal .. Fairy). */
static const uint8_t type_colors[PX_TYPE_COUNT][3] = {
    {168, 168, 120}, {192, 48, 40}, {168, 144, 240}, {160, 64, 160}, {224, 192, 104}, {184, 160, 56},
    {168, 184, 32}, {112, 88, 152}, {184, 184, 208}, {240, 128, 48}, {104, 144, 240}, {120, 200, 80},
    {248, 208, 48}, {248, 88, 136}, {152, 216, 216}, {112, 56, 248}, {112, 88, 72}, {238, 153, 172}};

static void ellipse(uint8_t *rgba, uint32_t w, uint32_t h, int cx, int cy, int rx, int ry, const uint8_t c[4])
{
    for (int y = -ry; y <= ry; ++y)
        for (int x = -rx; x <= rx; ++x)
            if ((double)x * x / ((double)rx * rx) + (double)y * y / ((double)ry * ry) <= 1.0)
                px_text_rect(rgba, w, h, cx + x, cy + y, cx + x + 1, cy + y + 1, c);
}

static void info_box(uint8_t *rgba, uint32_t w, uint32_t h, int x, int y, const struct px_battler *p,
                     const char *name, int show_numbers)
{
    static const uint8_t frame[4] = {64, 72, 88, 255}, paper[4] = {248, 248, 240, 255}, ink[4] = {40, 40, 48, 255};
    static const uint8_t empty[4] = {80, 88, 80, 255};
    px_text_rect(rgba, w, h, x, y, x + 150, y + (show_numbers ? 40 : 30), frame);
    px_text_rect(rgba, w, h, x + 2, y + 2, x + 148, y + (show_numbers ? 38 : 28), paper);
    char line[64];
    snprintf(line, sizeof line, "%s", name);
    px_text_draw(rgba, w, h, x + 6, y + 5, line, ink, 1);
    snprintf(line, sizeof line, "Lv%u", p->level);
    px_text_draw(rgba, w, h, x + 120, y + 5, line, ink, 1);
    const int bar = 100, filled = p->stats[0] ? (int)((uint32_t)bar * p->hp / p->stats[0]) : 0;
    const uint8_t *color = filled * 2 > bar ? (const uint8_t[4]){72, 200, 96, 255} :
                           filled * 5 > bar ? (const uint8_t[4]){232, 192, 48, 255} : (const uint8_t[4]){224, 72, 56, 255};
    px_text_draw(rgba, w, h, x + 6, y + 17, "HP", ink, 1);
    px_text_rect(rgba, w, h, x + 22, y + 18, x + 22 + bar + 2, y + 24, frame);
    px_text_rect(rgba, w, h, x + 23, y + 19, x + 23 + bar, y + 23, empty);
    px_text_rect(rgba, w, h, x + 23, y + 19, x + 23 + filled, y + 23, color);
    if (show_numbers) {
        snprintf(line, sizeof line, "%u/%u", p->hp, p->stats[0]);
        px_text_draw(rgba, w, h, x + 82, y + 28, line, ink, 1);
    }
}

void px_battle_ui_draw_stage(uint8_t *rgba, uint32_t w, uint32_t h, const struct px_battle *b, unsigned modeled)
{
    /* Background: a vertical gradient, ground below the horizon. */
    for (uint32_t y = 0; y < h; ++y) {
        const uint8_t sky[4] = {(uint8_t)(150 + 60 * y / h), (uint8_t)(200 + 40 * y / h), 240, 255};
        const uint8_t ground[4] = {(uint8_t)(120 + 40 * y / h), (uint8_t)(176 + 30 * y / h), 96, 255};
        px_text_rect(rgba, w, h, 0, (int)y, (int)w, (int)y + 1, y < h * 2 / 5 ? sky : ground);
    }
    static const uint8_t pad[4] = {96, 140, 72, 255};
    /* Wild Pokémon: far platform, upper right; the player's: near, lower left. */
    ellipse(rgba, w, h, (int)w * 3 / 4, (int)h * 9 / 20, 70, 16, pad);
    ellipse(rgba, w, h, (int)w / 4, (int)h * 3 / 4 - 8, 90, 20, pad);
    for (unsigned s = 0; s < 2; ++s) {
        const struct px_battler *p = &b->side[s];
        if (!p->hp || (s && b->state == PX_BATTLE_CAUGHT)) continue; /* fainted, or in the ball */
        if ((modeled >> s) & 1) continue; /* its model is drawn instead */
        const uint8_t *t = type_colors[p->types[0] < PX_TYPE_COUNT ? p->types[0] : 0];
        const uint8_t body[4] = {t[0], t[1], t[2], 255};
        const uint8_t shade[4] = {(uint8_t)(t[0] * 3 / 4), (uint8_t)(t[1] * 3 / 4), (uint8_t)(t[2] * 3 / 4), 255};
        const int cx = s ? (int)w * 3 / 4 : (int)w / 4, cy = s ? (int)h * 9 / 20 - 26 : (int)h * 3 / 4 - 40;
        const int r = s ? 24 : 32;
        ellipse(rgba, w, h, cx, cy, r, r, shade);
        ellipse(rgba, w, h, cx - 2, cy - 2, r - 3, r - 3, body);
    }
}

void px_battle_ui_draw_hud(uint8_t *rgba, uint32_t w, uint32_t h, const struct px_battle_ui *ui, const struct px_battle *b)
{
    char a[16], c[16];
    const char *wild = name_of(ui->species_names, ui->species_count, b->side[1].species, a, sizeof a);
    const char *mine = name_of(ui->species_names, ui->species_count, b->side[0].species, c, sizeof c);
    info_box(rgba, w, h, 16, 14, &b->side[1], wild, 0);
    info_box(rgba, w, h, (int)w - 166, (int)h * 3 / 5 - 30, &b->side[0], mine, 1);
    static const uint8_t frame[4] = {72, 80, 96, 255}, paper[4] = {250, 250, 246, 255}, ink[4] = {48, 48, 56, 255};
    static const uint8_t pick[4] = {224, 88, 64, 255};
    if (ui->phase == PX_UI_MESSAGES && ui->page < ui->page_count) {
        px_text_window(rgba, w, h, ui->pages[ui->page], ui->page + 1 < ui->page_count);
    } else if (ui->phase == PX_UI_COMMAND || ui->phase == PX_UI_MOVES) {
        const int y0 = (int)h - 50, y1 = (int)h - 4;
        px_text_rect(rgba, w, h, 8, y0, (int)w - 8, y1, frame);
        px_text_rect(rgba, w, h, 10, y0 + 2, (int)w - 10, y1 - 2, paper);
        if (ui->phase == PX_UI_COMMAND) {
            char line[96];
            snprintf(line, sizeof line, "What will %s do?", mine);
            px_text_draw(rgba, w, h, 18, y0 + 9, line, ink, 1);
            char ball[32];
            snprintf(ball, sizeof ball, "BALL x%u", (unsigned)b->balls);
            const int x[3] = {(int)w - 230, (int)w - 160, (int)w - 70};
            px_text_draw(rgba, w, h, x[0], y0 + 25, "FIGHT", ui->cursor == 0 ? pick : ink, 1);
            px_text_draw(rgba, w, h, x[1], y0 + 25, ball, ui->cursor == 1 ? pick : ink, 1);
            px_text_draw(rgba, w, h, x[2], y0 + 25, "RUN", ui->cursor == 2 ? pick : ink, 1);
            px_text_draw(rgba, w, h, x[ui->cursor >= 0 && ui->cursor < 3 ? ui->cursor : 0] - 10, y0 + 25, ">", pick, 1);
        } else {
            for (unsigned k = 0; k < 4; ++k) {
                const uint16_t id = b->side[0].moves[k];
                char line[64], buffer[16];
                if (id) {
                    const struct px_move *m = &b->data->moves[id];
                    snprintf(line, sizeof line, "%s %u/%u", name_of(ui->move_names, ui->move_count, id, buffer,
                                                                    sizeof buffer), b->side[0].pp[k], m->pp);
                } else snprintf(line, sizeof line, "-");
                const int x = 26 + (int)(k % 2) * ((int)w / 2 - 10), y = y0 + 9 + (int)(k / 2) * 16;
                px_text_draw(rgba, w, h, x, y, line, (int)k == ui->cursor ? pick : ink, 1);
                if ((int)k == ui->cursor) px_text_draw(rgba, w, h, x - 10, y, ">", pick, 1);
            }
        }
    }
}

void px_battle_ui_draw(uint8_t *rgba, uint32_t w, uint32_t h, const struct px_battle_ui *ui, const struct px_battle *b)
{
    px_battle_ui_draw_stage(rgba, w, h, b, 0);
    px_battle_ui_draw_hud(rgba, w, h, ui, b);
}
