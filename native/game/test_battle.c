#include "battle.h"
#include "battle_ui.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

static struct px_personal personal[4];
static struct px_move moves[6];
static uint8_t chart[PX_TYPE_COUNT][PX_TYPE_COUNT];

static void tables(void)
{
    memset(personal, 0, sizeof personal); memset(moves, 0, sizeof moves);
    for (unsigned i = 0; i < PX_TYPE_COUNT * PX_TYPE_COUNT; ++i) chart[i / PX_TYPE_COUNT][i % PX_TYPE_COUNT] = 4;
    chart[0][7] = 0;  /* Normal on Ghost */
    chart[10][9] = 8; /* Water on Fire */
    chart[10][4] = 8; /* Water on Ground */
    chart[10][11] = 2; /* Water on Grass */
    /* 1: Water, 2: Normal, 3: Ghost */
    const uint8_t base[3][6] = {{41, 56, 40, 71, 62, 44}, {38, 36, 38, 57, 32, 36}, {40, 40, 40, 10, 40, 40}};
    const uint8_t types[3] = {10, 0, 7};
    for (unsigned s = 1; s <= 3; ++s) {
        memcpy(personal[s].base, base[s - 1], 6);
        personal[s].types[0] = personal[s].types[1] = types[s - 1];
    }
    /* 1 Pound (Normal, physical 40), 2 Water Gun (Water, special 40), 3 Growl
     * (status, foe Attack -1), 4 Harden (status, self Defense +1), 5 a move
     * that never misses */
    moves[1] = (struct px_move){.type = 0, .category = 1, .power = 40, .accuracy = 100, .pp = 35};
    moves[2] = (struct px_move){.type = 10, .category = 2, .power = 40, .accuracy = 100, .pp = 25};
    moves[3] = (struct px_move){.type = 0, .category = 0, .accuracy = 100, .pp = 40, .target = 5, .stat = {1},
                                .stage = {-1}};
    moves[4] = (struct px_move){.type = 0, .category = 0, .accuracy = 101, .pp = 30, .target = 7, .stat = {2},
                                .stage = {1}};
    moves[5] = (struct px_move){.type = 0, .category = 1, .power = 60, .accuracy = 101, .pp = 20};
}

int main(void)
{
    tables();
    const struct px_battle_data data = {personal, 4, moves, 6, (const uint8_t (*)[PX_TYPE_COUNT])chart, NULL, NULL, 0, NULL, 0};
    const uint8_t iv[6] = {31, 31, 31, 31, 31, 31}, low[6] = {0, 0, 0, 0, 0, 0};
    const uint16_t set[4] = {1, 2, 3, 4};
    /* Constructor selection runs the original learnset getters and insertion. */
    const struct px_learnset initial = {.count = 7, .move = {1, 2, 3, 4, 5, 3, 1},
                                        .level = {1, 1, 1, 1, 2, 2, 3}};
    uint16_t selected[4];
    assert(px_battle_initial_moves(&data, &initial, 1, selected) && !memcmp(selected, set, sizeof set));
    assert(px_battle_initial_moves(&data, &initial, 2, selected) &&
           selected[0] == 2 && selected[1] == 3 && selected[2] == 4 && selected[3] == 5);
    assert(px_battle_initial_moves(&data, &initial, 3, selected) &&
           selected[0] == 3 && selected[1] == 4 && selected[2] == 5 && selected[3] == 1);
    assert(!px_battle_initial_moves(&data, &initial, 101, selected) && selected[0] == 0);
    struct px_learnset bounded = {.count = 27};
    for (unsigned k = 0; k < 27; ++k) { bounded.move[k] = 1; bounded.level[k] = 1; }
    bounded.move[26] = 99; /* excluded by the original loader's 26-entry limit */
    assert(px_battle_initial_moves(&data, &bounded, 5, selected) && selected[0] == 1);
    bounded.move[0] = 99;
    assert(!px_battle_initial_moves(&data, &bounded, 5, selected) && selected[0] == 0 && selected[3] == 0);
    struct px_battler a, b, g;
    /* Stats through the reconstructed formulas (neutral nature, no EVs). */
    assert(px_battler_make(&data, 1, 5, iv, 0, set, &a));
    assert(a.stats[0] == 20 && a.stats[1] == 12 && a.stats[2] == 10 && a.stats[3] == 13 && a.hp == 20);
    assert(a.pp[0] == 35 && a.pp[3] == 30);
    {
        struct px_battler named = a;
        const uint16_t utf16[] = {'F', 0x00e9, 0x2640, 0xd83d, 0xde00, 0, 'X'};
        assert(px_battler_set_name(&named, utf16, 7) && !memcmp(named.name, utf16, 6 * sizeof *utf16));
        assert(named.name[6] == 0 && named.species == a.species && named.hp == a.hp);
        const struct px_battler original = named;
        const uint16_t token[] = {'A', 0x10, 0, 0}, missing[] = {'X', 'Y'};
        assert(!px_battler_set_name(&named, token, 4) && !memcmp(&named, &original, sizeof named));
        assert(!px_battler_set_name(&named, missing, 2) && !memcmp(&named, &original, sizeof named));
        assert(!px_battler_set_name(&named, missing, 0) && !px_battler_set_name(&named, NULL, 0));
        assert(!px_battler_set_name(NULL, utf16, 7));
        /* An aliased source is captured before replacing/clearing the name. */
        assert(px_battler_set_name(&named, named.name + 1, 12) && named.name[0] == 0x00e9 && named.name[4] == 0);
    }
    assert(px_battler_make(&data, 2, 5, low, 0, set, &b) && b.stats[0] == 18 && b.stats[2] == 8);
    assert(!px_battler_make(&data, 4, 5, iv, 0, set, &g) && !px_battler_make(&data, 1, 0, iv, 0, set, &g));
    const uint16_t bad[4] = {1, 9, 0, 0};
    assert(!px_battler_make(&data, 1, 5, iv, 0, bad, &g));
    /* A nature table: nature 1 raises Attack and lowers Defense by 10%. */
    uint8_t natures[125] = {0};
    natures[5] = 1; natures[6] = 0xff;
    const struct px_battle_data natured = {personal, 4, moves, 6, (const uint8_t (*)[PX_TYPE_COUNT])chart, natures, NULL, 0, NULL, 0};
    assert(px_battler_make(&natured, 1, 50, iv, 1, set, &g) && g.stats[1] == 83 && g.stats[2] == 54);
    /* Damage: floor(floor(4 * 40 * 12 / 10) / 50) + 2 = 5 against Defense 10. */
    struct px_battler t = a;
    t.types[0] = t.types[1] = 0;
    uint32_t e;
    assert(px_battle_damage(&data, &a, &t, 1, 0, 100, &e) == 5 && e == 16);
    assert(px_battle_damage(&data, &a, &t, 1, 0, 85, &e) == 4);
    assert(px_battle_damage(&data, &a, &t, 1, 1, 100, &e) == 7);                 /* critical x1.5 */
    t.types[0] = t.types[1] = 9;
    assert(px_battle_damage(&data, &a, &t, 2, 0, 100, &e) == 5 * 3 / 2 * 2 && e == 32); /* STAB, super */
    t.types[0] = 9; t.types[1] = 4;
    assert(px_battle_damage(&data, &a, &t, 2, 0, 100, &e) == 7 * 4 && e == 64);  /* x4 */
    t.types[0] = t.types[1] = 11;
    assert(px_battle_damage(&data, &a, &t, 2, 0, 100, &e) == 3 && e == 8);       /* x0.5 */
    t.types[0] = t.types[1] = 7;
    assert(px_battle_damage(&data, &a, &t, 1, 0, 100, &e) == 0 && e == 0);       /* immune */
    struct px_battler up = a;
    up.stages[1] = 1;
    t.types[0] = t.types[1] = 0;
    assert(px_battle_stat(&up, 1, 0) == 18 && px_battle_damage(&data, &up, &t, 1, 0, 100, &e) == 7);
    up.stages[1] = -2;
    assert(px_battle_stat(&up, 1, 0) == 6 && px_battle_stat(&up, 1, 1) == 12); /* a critical hit ignores it */
    /* A battle: deterministic with a seed; PP and stages; it ends. */
    struct px_battle x, y;
    px_battle_start(&x, &data, &a, &b, 99);
    px_battle_start(&y, &data, &a, &b, 99);
    assert(px_battle_turn(&x, 2, NULL, 0, NULL, 0) && x.side[0].pp[2] == 39 && x.side[1].stages[1] == -1);
    assert(!strcmp(x.log[0], "#1 used move 3!") && !strcmp(x.log[1], "The wild #2's Attack fell!"));
    assert(x.log_event[0] == PX_LOG_STATUS && x.log_event[1] == PX_LOG_NONE); /* a status move, no damage */
    assert(px_battle_turn(&x, 3, NULL, 0, NULL, 0) && x.side[0].stages[2] == 1);
    assert(!px_battle_turn(&x, 4, NULL, 0, NULL, 0));         /* no such slot */
    x.side[0].pp[0] = 0;
    assert(!px_battle_turn(&x, 0, NULL, 0, NULL, 0));         /* no PP */
    x.side[0].pp[0] = 35;
    while (x.state == PX_BATTLE_CHOOSING && x.turn < 100) px_battle_turn(&x, 0, NULL, 0, NULL, 0);
    assert(x.state == PX_BATTLE_WON || x.state == PX_BATTLE_LOST);
    assert(x.side[x.state == PX_BATTLE_WON ? 1 : 0].hp == 0);
    {   /* The last turn's events: a damaging move flagged as hitting, then the faint of the loser. */
        int hit = 0, faint = -1;
        for (uint32_t i = 0; i < x.log_count; ++i) {
            if ((x.log_event[i] & PX_LOG_KIND) == PX_LOG_PHYSICAL && (x.log_event[i] & PX_LOG_HIT)) hit = 1;
            if ((x.log_event[i] & PX_LOG_KIND) == PX_LOG_FAINT) faint = (x.log_event[i] & PX_LOG_SIDE1) ? 1 : 0;
        }
        assert(hit && faint == (x.state == PX_BATTLE_WON ? 1 : 0));
    }
    assert(!px_battle_turn(&x, 0, NULL, 0, NULL, 0));         /* over */
    px_battle_turn(&y, 2, NULL, 0, NULL, 0); px_battle_turn(&y, 3, NULL, 0, NULL, 0);
    while (y.state == PX_BATTLE_CHOOSING && y.turn < 100) px_battle_turn(&y, 0, NULL, 0, NULL, 0);
    assert(y.turn == x.turn && y.state == x.state && y.side[0].hp == x.side[0].hp);
    /* A Ghost is immune to Pound. */
    assert(px_battler_make(&data, 3, 5, iv, 0, set, &g));
    px_battle_start(&x, &data, &a, &g, 5);
    px_battle_turn(&x, 0, NULL, 0, NULL, 0);
    assert(x.side[1].hp == x.side[1].stats[0]);
    int immune = 0;
    for (uint32_t i = 0; i < x.log_count; ++i) immune |= !strcmp(x.log[i], "It doesn't affect The wild #3...");
    assert(immune);
    /* Running: the faster side always gets away. */
    px_battle_start(&x, &data, &a, &b, 7);
    assert(px_battle_turn(&x, -1, NULL, 0, NULL, 0) && x.state == PX_BATTLE_ESCAPED && x.log_count == 1);
    /* The UI: intro pages, command menu, moves menu with cancel, running. */
    const char *species[] = {"", "Froakie", "Bunnelby", "Gastly"}, *names[] = {"", "Pound", "Water Gun", "Growl",
                                                                                 "Harden", "Strike"};
    struct px_battle_ui ui;
    px_battle_start(&x, &data, &a, &b, 11);
    px_battle_ui_start(&ui, &x, species, 4, names, 6, NULL);
    assert(ui.page_count == 2 && !strcmp(ui.pages[0], "A wild Bunnelby appeared!") && !strcmp(ui.pages[1], "Go! Froakie!"));
    assert(!px_battle_ui_input(&ui, &x, '.') && ui.phase == PX_UI_MESSAGES && ui.page == 1);
    px_battle_ui_input(&ui, &x, '.');
    assert(ui.phase == PX_UI_COMMAND && ui.cursor == 0);
    px_battle_ui_input(&ui, &x, '.');
    assert(ui.phase == PX_UI_MOVES);
    px_battle_ui_input(&ui, &x, 'R'); px_battle_ui_input(&ui, &x, 'D');
    assert(ui.cursor == 3);
    px_battle_ui_input(&ui, &x, 'x');
    assert(ui.phase == PX_UI_COMMAND);
    px_battle_ui_input(&ui, &x, '.'); px_battle_ui_input(&ui, &x, 'D');
    assert(ui.cursor == 2 && px_battle_ui_input(&ui, &x, '.') == 1); /* Growl */
    assert(ui.phase == PX_UI_MESSAGES && !strcmp(ui.pages[0], "Froakie used Growl!"));
    assert(ui.page_event[0] == PX_LOG_STATUS);
    while (ui.phase == PX_UI_MESSAGES) px_battle_ui_input(&ui, &x, '.');
    assert(ui.phase == PX_UI_COMMAND);
    for (unsigned k = 0; k < 4; ++k) px_battle_ui_input(&ui, &x, 'R'); /* FIGHT BALL POKéMON RUN */
    assert(ui.cursor == 3 && px_battle_ui_input(&ui, &x, '.') == 1 && !strcmp(ui.pages[0], "Got away safely!"));
    px_battle_ui_input(&ui, &x, '.');
    assert(ui.phase == PX_UI_DONE && x.state == PX_BATTLE_ESCAPED);
    /* BALL without balls: a message, back to the menu, no turn. */
    px_battle_start(&x, &data, &a, &b, 11);
    px_battle_ui_start(&ui, &x, species, 4, names, 6, NULL);
    px_battle_ui_input(&ui, &x, '.'); px_battle_ui_input(&ui, &x, '.'); px_battle_ui_input(&ui, &x, 'R');
    assert(ui.cursor == 1 && px_battle_ui_input(&ui, &x, '.') == 0 && x.turn == 0 &&
           !strcmp(ui.pages[0], "You have no Poké Balls left!"));
    px_battle_ui_input(&ui, &x, '.');
    assert(ui.phase == PX_UI_COMMAND);
    static uint8_t rgba[400 * 240 * 4];
    px_battle_ui_draw(rgba, 400, 240, &ui, &x); /* draws without a page or menu */
    {   /* The party: a switch is a turn; the list refuses the active and fainted ones. */
        struct px_battler team[3], other;
        assert(px_battler_make(&data, 2, 5, iv, 0, set, &other));
        team[0] = a; team[1] = other; team[2] = other; team[2].hp = 0;
        px_battle_start(&x, &data, &a, &b, 21);
        assert(!px_battle_set_party(&x, team, 3, 2) && !px_battle_set_party(&x, team, 7, 0));
        assert(px_battle_set_party(&x, team, 3, 0) && x.party_count == 3 && x.active == 0);
        px_battle_ui_start(&ui, &x, species, 4, names, 6, NULL);
        px_battle_ui_input(&ui, &x, '.'); px_battle_ui_input(&ui, &x, '.');
        px_battle_ui_input(&ui, &x, 'R'); px_battle_ui_input(&ui, &x, 'R');
        px_battle_ui_input(&ui, &x, '.');
        assert(ui.phase == PX_UI_PARTY && !ui.forced && ui.cursor == 0);
        px_battle_ui_draw(rgba, 400, 240, &ui, &x);
        assert(px_battle_ui_input(&ui, &x, '.') == 0 && strstr(ui.note, "already in battle"));
        px_battle_ui_input(&ui, &x, 'D'); px_battle_ui_input(&ui, &x, 'D');
        assert(px_battle_ui_input(&ui, &x, '.') == 0 && strstr(ui.note, "no energy left"));
        px_battle_ui_input(&ui, &x, 'x');
        assert(ui.phase == PX_UI_COMMAND && ui.cursor == 2);
        px_battle_ui_input(&ui, &x, '.'); px_battle_ui_input(&ui, &x, 'D');
        assert(px_battle_ui_input(&ui, &x, '.') == 1 && x.active == 1 && x.side[0].species == 2 && x.turn == 1);
        assert(!strcmp(ui.pages[0], "Come back, Froakie!") && !strcmp(ui.pages[1], "Go, Bunnelby!") &&
               ui.page_event[1] == PX_LOG_SEND && ui.page_count >= 3); /* then the wild Pokémon moves */
        assert(x.party[0].species == 1 && x.party[0].hp == a.hp && x.party[1].hp == x.side[0].hp);
        /* A faint with another able Pokémon: the list must choose, no turn. */
        x.side[0].hp = 1; x.side[0].stats[3] = 1; x.side[1].moves[0] = 5; x.side[1].moves[1] = 0;
        x.side[1].moves[2] = x.side[1].moves[3] = 0;
        while (ui.phase == PX_UI_MESSAGES) px_battle_ui_input(&ui, &x, '.');
        px_battle_ui_input(&ui, &x, '.'); /* FIGHT */
        px_battle_ui_input(&ui, &x, '.'); /* Pound; the wild never-miss move faints it */
        assert(x.state == PX_BATTLE_SWITCH && x.party[1].hp == 0);
        while (ui.phase == PX_UI_MESSAGES) px_battle_ui_input(&ui, &x, '.');
        assert(ui.phase == PX_UI_PARTY && ui.forced && ui.cursor == 0);
        px_battle_ui_input(&ui, &x, 'x');
        assert(ui.phase == PX_UI_PARTY);
        const uint32_t turn = x.turn;
        assert(px_battle_ui_input(&ui, &x, '.') == 1 && x.active == 0 && x.turn == turn &&
               x.state == PX_BATTLE_CHOOSING && ui.page_count == 1 && !strcmp(ui.pages[0], "Go, Froakie!"));
        /* The last able one faints: lost. */
        x.side[0].hp = 1; x.side[0].stats[3] = 1;
        const int growled = px_battle_turn(&x, 2, species, 4, names, 6);
        assert(growled && x.state == PX_BATTLE_LOST &&
               !strcmp(x.log[x.log_count - 1], "You have no more Pokémon that can fight!"));
        assert(!px_battle_send_out(&x, 1, species, 4, names, 6));
    }
    {   /* The touch screen: taps choose buttons, any tap turns a page. */
        static uint8_t bottom[320 * 240 * 4];
        px_battle_start(&x, &data, &a, &b, 11);
        px_battle_ui_start(&ui, &x, species, 4, names, 6, NULL);
        assert(px_battle_ui_touch(&ui, &x, 5, 5) == 0 && ui.page == 1);
        px_battle_ui_touch(&ui, &x, 300, 230);
        assert(ui.phase == PX_UI_COMMAND);
        px_battle_ui_draw_touch(bottom, 320, 240, &ui, &x);
        assert(px_battle_ui_touch(&ui, &x, 5, 235) == 0 && ui.phase == PX_UI_COMMAND); /* between buttons */
        px_battle_ui_touch(&ui, &x, 160, 60);                                          /* FIGHT */
        assert(ui.phase == PX_UI_MOVES);
        px_battle_ui_draw_touch(bottom, 320, 240, &ui, &x);
        px_battle_ui_touch(&ui, &x, 160, 215);                                         /* BACK */
        assert(ui.phase == PX_UI_COMMAND);
        px_battle_ui_touch(&ui, &x, 160, 60);
        assert(px_battle_ui_touch(&ui, &x, 80, 140) == 1 && !strcmp(ui.pages[0], "Froakie used Growl!")); /* slot 2 */
        while (ui.phase == PX_UI_MESSAGES) px_battle_ui_touch(&ui, &x, 1, 1);
        assert(px_battle_ui_touch(&ui, &x, 160, 206) == 1 && !strcmp(ui.pages[0], "Got away safely!"));   /* RUN */
        px_battle_ui_draw_touch(bottom, 320, 240, &ui, &x);
    }
    /* Experience and levels: growth group 0 is level^3 (a/2/1/7 layout). */
    static uint32_t growth[1][101];
    for (unsigned l = 0; l <= 100; ++l) growth[0][l] = l * l * l;
    static struct px_learnset learnsets[4];
    learnsets[1].count = 2; learnsets[1].move[0] = 1; learnsets[1].level[0] = 1;
    learnsets[1].move[1] = 5; learnsets[1].level[1] = 8;
    personal[2].base_exp = 255; personal[2].catch_rate = 255;
    const struct px_battle_data grown = {personal, 4, moves, 6, (const uint8_t (*)[PX_TYPE_COUNT])chart, NULL,
                                         (const uint32_t (*)[101])growth, 1, learnsets, 4};
    assert(px_battle_level_for(&grown, 1, 124) == 4 && px_battle_level_for(&grown, 1, 125) == 5 &&
           px_battle_level_for(&grown, 1, 1000000) == 100 && px_battle_level_for(&grown, 1, 0) == 1);
    const uint16_t two[4] = {1, 0, 0, 0};
    struct px_battler weak, foe;
    assert(px_battler_make(&grown, 1, 5, iv, 0, two, &weak) && weak.exp == 125);
    assert(px_battler_make(&grown, 2, 30, low, 0, two, &foe));
    assert(px_battle_exp_gain(&grown, &foe) == 255 * 30 / 7);
    foe.hp = 1; /* one Pound faints it */
    px_battle_start(&x, &grown, &weak, &foe, 3);
    x.side[1].stats[3] = 1; /* slower: the partner moves first */
    px_battle_turn(&x, 0, species, 4, names, 6);
    assert(x.state == PX_BATTLE_WON && x.side[0].exp == 125 + 1092 && x.side[0].level == 10);
    assert(x.side[0].moves[1] == 5 && x.side[0].pp[1] == 20); /* learnt at level 8 */
    struct px_battler ten;
    px_battler_make(&grown, 1, 10, iv, 0, two, &ten);
    assert(x.side[0].stats[0] == ten.stats[0] && x.side[0].hp == ten.stats[0] - (weak.stats[0] - weak.hp));
    int grew = 0, learnt = 0;
    for (uint32_t i = 0; i < x.log_count; ++i) {
        grew += !strncmp(x.log[i], "Froakie grew to Lv. ", 20);
        learnt |= !strcmp(x.log[i], "Froakie learned Strike!");
    }
    assert(grew == 5 && learnt && !strcmp(x.log[x.log_count - 7], "Froakie gained 1092 Exp. Points!"));
    /* Catching: a = floor((3M - 2H) x rate x ball / 3M); 255 or more catches at once. */
    struct px_battler full = foe;
    full.hp = full.stats[0];
    assert(px_battle_catch_threshold(&grown, &full, 10) == 53335); /* a = 85 */
    assert(px_battle_catch_threshold(&grown, &full, 30) == 65536);
    personal[2].catch_rate = 0;
    assert(px_battle_catch_threshold(&grown, &full, 10) == 0);
    personal[2].catch_rate = 255;
    px_battle_start(&x, &grown, &weak, &full, 21);
    x.balls = 50;
    while (x.state == PX_BATTLE_CHOOSING && x.balls) px_battle_turn(&x, -2, species, 4, names, 6);
    assert(x.state == PX_BATTLE_CAUGHT && x.balls < 50 && !strcmp(x.log[x.log_count - 1], "Gotcha! Bunnelby was caught!"));
    x.state = PX_BATTLE_CHOOSING; x.balls = 0;
    assert(!px_battle_turn(&x, -2, NULL, 0, NULL, 0)); /* no balls */
    struct px_rng r = {0};
    const uint32_t first = px_rng_next(&r);
    assert(first && px_rng_below(&r, 1) == 0 && px_rng_below(&r, 0) == 0);
    puts("battle tests passed");
    return 0;
}
