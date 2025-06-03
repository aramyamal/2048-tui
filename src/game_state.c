#ifndef GAME_STATE_C
#define GAME_STATE_C

#include "uint32_array.c"
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

uint32_t GameState_get(const GameState *gs, size_t i, size_t j) {
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

GameState *GameState_create(size_t dim, size_t undos) {

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
        .prev_left = undos,
        .prev = NULL,
        .score = 0,
    };
    GameState_add_random(game_state);
    GameState_add_random(game_state);
    return game_state;
}

void GameState_destroy_single(GameState *gs) {
    UInt32Array_destroy(&gs->tiles);
    free(gs);
}

void GameState_destroy_chain(GameState *gs) {
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
        GameState_destroy_single(gs);
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

    UInt32Array new_tiles = UInt32Array_copy(gs->tiles);
    if (!new_tiles.items) {
        free(copy);
        return NULL;
    }

    *copy = (GameState){
        .tiles = new_tiles,
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

void GameState_cleanup_old_states(GameState *gs) {
    if (!gs || gs->prev_left == 0) {
        return;
    }

    size_t remaining = gs->prev_left;
    GameState *current = gs;

    for (size_t i = 0; i < remaining && current->prev; ++i) {
        current = current->prev;
    }

    if (current->prev) {
        GameState_destroy_chain(current->prev);
        current->prev = NULL;
    }
}

GameState *GameState_undo(GameState *gs) {
    if (!gs || !gs->prev || gs->prev_left == 0) {
        return NULL;
    }

    size_t current_prev_left = gs->prev_left;
    GameState *prev = gs->prev;
    GameState_destroy_single(gs);

    prev->prev_left = current_prev_left - 1;

    return prev;
}

bool GameState_equals(const GameState *gs1, const GameState *gs2) {
    if (!gs1 || !gs2) {
        return gs1 == gs2;
    }

    if (gs1->dim != gs2->dim) {
        return false;
    }

    for (size_t row = 0; row < gs1->dim; ++row) {
        for (size_t col = 0; col < gs1->dim; ++col) {
            if (GameState_get(gs1, row, col) != GameState_get(gs2, row, col)) {
                return false;
            }
        }
    }
    return true;
}

GameState *GameState_slide_and_merge_right(GameState *gs) {
    if (!gs) {
        return NULL;
    }

    GameState *new_gs = GameState_copy(gs);
    if (!new_gs) {
        return NULL;
    }

    // slide all tiles
    GameState_slide_right(new_gs);

    // merge tiles and add scores
    new_gs->score += GameState_merge_right(new_gs);

    // slide again after merging
    GameState_slide_right(new_gs);

    // check if anything changed
    if (GameState_equals(gs, new_gs)) {
        GameState_destroy_chain(new_gs);
        return NULL;
    }

    // otherwise valid mode, set correct previous chain
    new_gs->prev = gs;

    // remove unaccessible previous game states
    GameState_cleanup_old_states(new_gs);

    return new_gs;
}

void GameState_transpose(GameState *gs) {
    size_t dim = gs->dim;
    for (size_t i = 0; i < dim; ++i) {
        for (size_t j = i + 1; j < dim; ++j) {
            uint32_t temp = GameState_get(gs, i, j);
            GameState_set(gs, i, j, GameState_get(gs, j, i));
            GameState_set(gs, j, i, temp);
        }
    }
}

void GameState_reverse_rows(GameState *gs) {
    size_t dim = gs->dim;
    for (size_t i = 0; i < dim; ++i) {
        for (size_t j = 0; j < dim / 2; ++j) {
            uint32_t temp = GameState_get(gs, i, j);
            GameState_set(gs, i, j, GameState_get(gs, i, dim - 1 - j));
            GameState_set(gs, i, dim - 1 - j, temp);
        }
    }
}

void GameState_reverse_cols(GameState *gs) {
    size_t dim = gs->dim;
    for (size_t j = 0; j < dim; ++j) {
        for (size_t i = 0; i < dim / 2; ++i) {
            uint32_t temp = GameState_get(gs, i, j);
            GameState_set(gs, i, j, GameState_get(gs, i, dim - 1 - j));
            GameState_set(gs, i, dim - 1 - j, temp);
        }
    }
}

void GameState_rotate90(GameState *gs) {
    GameState_transpose(gs);
    GameState_reverse_rows(gs);
}

void GameState_rotate180(GameState *gs) {
    GameState_reverse_rows(gs);
    GameState_reverse_cols(gs);
}

void GameState_rotate270(GameState *gs) {
    GameState_reverse_rows(gs);
    GameState_transpose(gs);
}

GameState *GameState_slide_and_merge_left(GameState *gs) {

    if (!gs) {
        return NULL;
    }

    GameState *new_gs = GameState_copy(gs);
    if (!new_gs) {
        return NULL;
    }

    GameState_rotate180(new_gs);
    GameState_slide_right(new_gs);
    new_gs->score += GameState_merge_right(new_gs);
    GameState_slide_right(new_gs);
    GameState_rotate180(new_gs);

    if (GameState_equals(gs, new_gs)) {
        GameState_destroy_chain(new_gs);
        return NULL;
    }

    new_gs->prev = gs;

    // remove unaccessible previous game states
    GameState_cleanup_old_states(new_gs);

    return new_gs;
}

GameState *GameState_slide_and_merge_up(GameState *gs) {
    if (!gs) {
        return NULL;
    }

    GameState *new_gs = GameState_copy(gs);
    if (!new_gs) {
        return NULL;
    }

    GameState_rotate90(new_gs);
    GameState_slide_right(new_gs);
    new_gs->score += GameState_merge_right(new_gs);
    GameState_slide_right(new_gs);
    GameState_rotate270(new_gs);

    if (GameState_equals(gs, new_gs)) {
        GameState_destroy_chain(new_gs);
        return NULL;
    }

    new_gs->prev = gs;

    // remove unaccessible previous game states
    GameState_cleanup_old_states(new_gs);

    return new_gs;
}

GameState *GameState_slide_and_merge_down(GameState *gs) {
    if (!gs) {
        return NULL;
    }

    GameState *new_gs = GameState_copy(gs);
    if (!new_gs) {
        return NULL;
    }

    GameState_rotate270(new_gs);
    GameState_slide_right(new_gs);
    new_gs->score += GameState_merge_right(new_gs);
    GameState_slide_right(new_gs);
    GameState_rotate90(new_gs);

    if (GameState_equals(gs, new_gs)) {
        GameState_destroy_chain(new_gs);
        return NULL;
    }

    new_gs->prev = gs;

    // remove unaccessible previous game states
    GameState_cleanup_old_states(new_gs);

    return new_gs;
}

bool GameState_can_move(GameState *gs) {
    size_t dim = gs->dim;
    for (size_t row = 0; row < dim; ++row) {
        for (size_t col = 0; col < dim; ++col) {
            uint32_t current = GameState_get(gs, row, col);

            if (current == 0) {
                return true;
            }

            // check if current tile can merge with right neighbour
            if (col < dim - 1 && current == GameState_get(gs, row, col + 1)) {
                return true;
            }

            // check if current tile can merge with bottom
            if (row < dim - 1 && current == GameState_get(gs, row + 1, col)) {
                return true;
            }
        }
    }
    // no merges or moves possible
    return false;
}

#endif // GAME_STATE_C
