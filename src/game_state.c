#ifndef GAME_STATE_C
#define GAME_STATE_C

#include "uint32_array.c"
#include <ncurses.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

typedef struct GameState GameState;
struct GameState {
    UInt32Array tiles;
    size_t dim;
    size_t prev_left;
    GameState *prev;
    uint32_t score;
};

uint32_t GameState_get(GameState *gs, size_t i, size_t j) {
    return UInt32Array_get(gs->tiles, (i * gs->dim) + j);
}

bool GameState_set(GameState *gs, size_t i, size_t j, uint32_t val) {
    return UInt32Array_set(&gs->tiles, (i * gs->dim) + j, val);
}

bool GameState_add_random(GameState *gs) {
    typedef struct {
        size_t i;
        size_t j;
    } index;

    size_t dim = gs->dim;
    size_t zero_count = 0;

    // count number of empty tiles
    for (size_t i = 0; i < dim; ++i) {
        for (size_t j = 0; j < dim; ++j) {
            if (GameState_get(gs, i, j) == 0) {
                zero_count++;
            }
        }
    }

    if (zero_count == 0) {
        return false;
    }

    // allocate array for storing empty tile indices
    index *avail_idxs = malloc(zero_count * sizeof(index));
    if (!avail_idxs) {
        return false;
    }

    size_t current_idx = 0;
    for (size_t i = 0; i < dim; i++) {
        for (size_t j = 0; j < dim; j++) {
            if (GameState_get(gs, i, j) == 0) {
                avail_idxs[current_idx++] = (index){.i = i, .j = j};
            }
        }
    }

    // randomly pick index of empty tiles to add either 2 or 4 to
    size_t random_idx = rand() % zero_count;
    index new_tile = avail_idxs[random_idx];
    uint32_t value = (rand() % 10) < 9 ? 2 : 4;
    GameState_set(gs, new_tile.i, new_tile.j, value);

    free(avail_idxs);
    return true;
}

GameState *GameState_create(size_t dim) {

    GameState *game_state = malloc(sizeof(GameState));
    if (game_state == NULL) {
        return NULL;
    }

    UInt32Array tiles = UInt32Array_create(dim * dim, dim * dim);
    if (tiles.items == NULL) {
        free(game_state);
        return NULL;
    }

    *game_state = (GameState){
        .tiles = tiles,
        .dim = dim,
        .prev_left = 3,
        .prev = NULL,
        .score = 0,
    };
    GameState_add_random(game_state);
    GameState_add_random(game_state);
    return game_state;
}

void GameState_destroy(GameState *gs) {
    // if (gs) {
    //     UInt32Array_destroy(gs->tiles);
    //
    //     // remove previous GameState chain
    //     if (gs->previous != NULL) {
    //         GameState_destroy(gs->previous);
    //     }
    //     free(gs);
    // }

    // better iterative approach
    while (gs != NULL) {
        GameState *prev = gs->prev;
        UInt32Array_destroy(&gs->tiles);
        free(gs);
        gs = prev;
    }
}

GameState *GameState_copy(GameState *gs) {
    if (gs == NULL) {
        return NULL;
    }

    GameState *copy = malloc(sizeof(GameState));
    if (copy == NULL) {
        return NULL;
    }

    *copy = (GameState){
        .tiles = UInt32Array_copy(gs->tiles),
        .dim = gs->dim,
        .prev_left = gs->prev_left,
        .prev = NULL,
        .score = gs->score,
    };

    return copy;
}

void GameState_slide_right(GameState *gs) {
    size_t dim = gs->dim;

    for (size_t row = 0; row < dim; ++row) {
        for (size_t i = 0; i < dim - 1; ++i) {
            size_t col = dim - 2 - i; // this gives dim-2, dim-3, ..., 1, 0

            uint32_t tile = GameState_get(gs, row, col);
            if (tile == 0) {
                continue;
            }

            size_t target_col = col;
            while (target_col + 1 < dim &&
                   GameState_get(gs, row, target_col + 1) == 0) {
                ++target_col;
            }

            if (target_col != col) {
                GameState_set(gs, row, target_col, tile);
                GameState_set(gs, row, col, 0);
            }
        }
    }
}

uint32_t GameState_merge_right(GameState *gs) {
    uint32_t score_add = 0;
    size_t dim = gs->dim;

    // merge tiles from right to left
    for (size_t row = 0; row < dim; ++row) {
        for (size_t col = dim - 1; col > 0; --col) {
            uint32_t right_tile = GameState_get(gs, row, col);
            uint32_t left_tile = GameState_get(gs, row, col - 1);
            if (right_tile == left_tile && right_tile != 0) {
                uint32_t merged_value = right_tile * 2;
                GameState_set(gs, row, col, merged_value);
                GameState_set(gs, row, col - 1, 0);
                score_add += merged_value;
            }
        }
    }
    return score_add;
}

GameState *GameState_slide_and_merge_right(GameState *gs) {
    size_t dim = gs->dim;

    GameState *new_gs = GameState_copy(gs);
    new_gs->prev = gs;

    // slide all tiles
    GameState_slide_right(new_gs);

    // merge tiles and add scores
    new_gs->score += GameState_merge_right(new_gs);

    // slide again after merging
    GameState_slide_right(new_gs);

    // remove unaccessible previous game states
    if (new_gs->prev_left > 0) {
        GameState *current = new_gs;
        for (size_t i = 0; i < new_gs->prev_left - 1 && current->prev; ++i) {
            current = current->prev;
        }
        if (current->prev) {
            GameState_destroy(current->prev);
            current->prev = NULL;
        }
    }
    return new_gs;
}

void GameState_print(GameState *gs) {
    if (!gs) {
        printw("Game state is NULL\n");
        return;
    }

    size_t dim = gs->dim;

    printw("Score: %u\n\n", gs->score);

    for (size_t i = 0; i < dim; i++) {
        for (size_t j = 0; j < dim; j++) {
            uint32_t value = GameState_get(gs, i, j);
            if (value == 0) {
                printw("[    ]");
            } else {
                printw("[%4u]", value);
            }
            if (j < dim - 1) {
                printw(" ");
            }
        }
        printw("\n");
        if (i < dim - 1) {
            printw("\n"); // extra space between rows
        }
    }
    printw("\n");
}

#endif // GAME_STATE_C
