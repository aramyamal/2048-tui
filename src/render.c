#ifndef RENDER_C
#define RENDER_C

#define LIGHT_HUE 8
#define NR_OF_COLORS 14
#define LIGHT_THRES 8

#include "game_state.c"
#include <ncurses.h>
#include <stdbool.h>
#include <stdint.h>

static void ensure_colors_initialized(void) {
    static bool initiated = false;
    if (initiated) {
        return;
    }
    initiated = true;

    int8_t bg_colors[NR_OF_COLORS] = {
        0,
        COLOR_WHITE,
        COLOR_RED,
        COLOR_YELLOW,
        COLOR_GREEN,
        COLOR_CYAN,
        COLOR_BLUE,
        COLOR_MAGENTA,
        COLOR_RED + LIGHT_HUE,
        COLOR_YELLOW + LIGHT_HUE,
        COLOR_GREEN + LIGHT_HUE,
        COLOR_CYAN + LIGHT_HUE,
        COLOR_BLUE + LIGHT_HUE,
        COLOR_MAGENTA + LIGHT_HUE,
    };

    start_color();
    use_default_colors();

    for (size_t i = 0; i < NR_OF_COLORS; i++) {
        if (i < LIGHT_THRES) {
            // regular colors: foreground = bg_colors[i]
            init_pair(i + 1, bg_colors[i], -1);
        } else {
            // bright colours: background = bg_colors[i] //
            init_pair(i + 1, -1, bg_colors[i]);
        }
    }
    // default for other numbers
    init_pair(NR_OF_COLORS + 1, -1, COLOR_WHITE + LIGHT_HUE);
}

static int pair_for_value(uint32_t value) {
    if (value == 0) {
        return 0;
    }
    for (size_t k = 0; k < NR_OF_COLORS; k++) {
        if (value == (1U << k)) {
            return k + 1;
        }
    }
    return NR_OF_COLORS + 1;
}

void GameState_print(GameState *gs) {
    if (!gs) {
        printw("Game state is NULL\n");
        return;
    }

    ensure_colors_initialized();

    const size_t dim = gs->dim;
    const int cell_w = 7;
    const int cell_h = 3;

    printw("\n");
    printw("╭───────────────────────────────╮\n");
    printw("│ Score: %-10u", gs->score);
    if (gs->prev_left != 0) {
        printw("  Undos: %-3zu │\n", gs->prev_left);
    } else {
        printw("             │\n");
    }
    printw("╰───────────────────────────────╯\n");

    // draw the grid and contents
    for (size_t i = 0; i <= dim; i++) {
        // draw one horizontal border line
        for (size_t j = 0; j <= dim; j++) {
            // choose corner/junction glyph
            char *glyph = NULL;
            if (i == 0 && j == 0) {
                glyph = "╭";

            } else if (i == 0 && j == dim) {
                glyph = "╮";

            } else if (i == dim && j == 0) {
                glyph = "╰";

            } else if (i == dim && j == dim) {
                glyph = "╯";

            } else if (i == 0) {
                glyph = "┬";

            } else if (i == dim) {
                glyph = "┴";

            } else if (j == 0) {
                glyph = "├";

            } else if (j == dim) {
                glyph = "┤";

            } else {
                glyph = "┼";
            }

            printw("%s", glyph);
            if (j < dim) {
                for (int k = 0; k < cell_w; k++) {
                    printw("─");
                }
            }
        }
        printw("\n");

        // draw the cell_h content-rows for each row of tiles
        if (i < dim) {
            for (int row = 0; row < cell_h; row++) {
                for (size_t j = 0; j < dim; j++) {
                    printw("│");
                    uint32_t val = GameState_get(gs, i, j);

                    // center on the middle content row
                    if (row == cell_h / 2 && val != 0) {
                        char buf[16];
                        snprintf(buf, sizeof(buf), "%u", val);
                        int len = strlen(buf);
                        int padL = (cell_w - len) / 2;
                        int padR = cell_w - len - padL;

                        int pair = pair_for_value(val);
                        if (pair != 0) {
                            if (pair <= LIGHT_THRES) {
                                attron(COLOR_PAIR(pair) | A_REVERSE);
                            } else {
                                attron(COLOR_PAIR(pair));
                            }
                        }

                        printw("%*s%s%*s", padL, "", buf, padR, "");

                        if (pair != 0) {
                            if (pair <= LIGHT_THRES) {
                                attroff(COLOR_PAIR(pair) | A_REVERSE);
                            } else {
                                attroff(COLOR_PAIR(pair));
                            }
                        }
                    } else {
                        // blank or full-background line
                        int pair = pair_for_value(val);
                        if (pair != 0) {
                            if (pair <= LIGHT_THRES) {
                                attron(COLOR_PAIR(pair) | A_REVERSE);
                            } else {
                                attron(COLOR_PAIR(pair));
                            }
                        }

                        printw("%*s", cell_w, "");

                        if (pair != 0) {
                            if (pair <= LIGHT_THRES) {
                                attroff(COLOR_PAIR(pair) | A_REVERSE);
                            } else {
                                attroff(COLOR_PAIR(pair));
                            }
                        }
                    }
                }
                printw("│\n");
            }
        }
    }
}

#endif // RENDER_C
