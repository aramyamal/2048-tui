#include "game_state.c"
#include "render.c"
#include <locale.h>
#include <ncurses.h>
#include <stdint.h>
#include <time.h>

#define DEFAULT_DIMENSION 4
#define DEFAULT_UNDOS 3
#define BASE_TEN 10

// helper to parse positive integer
int32_t parse_positive(const char *s, int32_t min) {
    char *end = NULL;
    int32_t val = strtol(s, &end, BASE_TEN);
    if (*end != '\0' || val < min) {
        return -1;
    }
    return val;
}

int main(int32_t argc, char *argv[]) {
    int dimension = DEFAULT_DIMENSION;
    int undos = DEFAULT_UNDOS;

    // command line arguments
    for (size_t i = 1; i < argc; ++i) {
        if ((strcmp(argv[i], "-d") == 0 ||
             strcmp(argv[i], "--dimension") == 0) &&
            i + 1 < argc) {
            int val = parse_positive(argv[i + 1], 3);
            if (val == -1) {
                fprintf(stderr, "Error: Dimension must be an integer > 2\n");
                return 1;
            }
            dimension = val;
            ++i;
        } else if ((strcmp(argv[i], "-u") == 0 ||
                    strcmp(argv[i], "--undos") == 0) &&
                   i + 1 < argc) {
            int val = parse_positive(argv[i + 1], 0);
            if (val == -1) {
                fprintf(stderr, "Error: Undos must be an integer >= 0\n");
                return 1;
            }
            undos = val;
            ++i;
        } else {
            fprintf(stderr, "Unknown argument: %s\n", argv[i]);
            fprintf(stderr,
                    "Usage: %s [-d n | --dimension n] [-u n | --undos n]\n",
                    argv[0]);
            return 1;
        }
    }
    // set locale for unicode support
    setlocale(LC_ALL, "");

    // initialize ncurses
    initscr();
    cbreak();             // disable line buffering
    noecho();             // don't echo pressed keys
    keypad(stdscr, TRUE); // enable special keys

    srand(time(NULL));
    GameState *gs = GameState_create(dimension, undos);

    // clear screen and print initial state
    clear();
    GameState_print(gs);
    printw("╭╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╮\n");
    printw("╎ Choose slide direction with:  ╎\n");
    printw("╎                               ╎\n");
    printw("╎               w        ↑      ╎\n");
    printw("╎  h j k l,   a s d,   ← ↓ →.   ╎\n");
    printw("╰╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╯\n");

    printw("╭╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╮\n");
    printw("╎ Undo with:     u, z, space.   ╎\n");
    printw("╰╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╌╯\n");
    refresh();

    int32_t ch = 0;
    bool game_over = false;
    bool exit = false;

    bool last_move_was_undo = false;
    GameState *new_gs = NULL;
    while (!exit && (ch = getch()) != 'q') {
        last_move_was_undo = false;

        switch (ch) {
        case KEY_LEFT:
        case 'a':
        case 'h':
            new_gs = GameState_slide_and_merge_left(gs);
            break;
        case KEY_DOWN:
        case 's':
        case 'j':
            new_gs = GameState_slide_and_merge_down(gs);
            break;
        case KEY_UP:
        case 'w':
        case 'k':
            new_gs = GameState_slide_and_merge_up(gs);
            break;
        case KEY_RIGHT:
        case 'd':
        case 'l':
            new_gs = GameState_slide_and_merge_right(gs);
            break;
        case 'u':
        case 'z':
        case ' ':
            new_gs = GameState_undo(gs);
            last_move_was_undo = true;
            break;
        default:
            printw("unknown key '%c'\n", ch);
            break;
        }

        // if change occured
        if (new_gs) {
            if (!last_move_was_undo) {
                game_over = !GameState_add_random(new_gs) ||
                            !GameState_can_move(new_gs);
            }
            gs = new_gs;
        }

        // clear screen and redraw
        clear();
        GameState_print(gs);
        refresh();

        // check for game over after each move
        if (game_over) {
            printw("\nGame Over! Press 'q' to quit");
            char re = 0;

            // if undos still left, allow undo to revocer
            if (gs->prev_left > 0) {
                printw(" or 'u/z/space' to undo.\n");
                do {
                    re = getch();
                } while (re != 'q' && re != 'u' && re != 'z' && re != ' ');
                // if quitting, exit the loop
                if (re == 'q') {
                    exit = true;
                } else { // if undoing, undo and redraw
                    gs = GameState_undo(gs);
                    game_over = false;
                    clear();
                    GameState_print(gs);
                    refresh();
                }
            } else { // if no undos left, exit game on 'q' keypress
                printw(".\n");
                do {
                    re = getch();
                } while (re != 'q');
                exit = true;
            }
        }
    }

    // cleanup ncurses
    endwin();
    // cleanup game state
    GameState_destroy_chain(gs);
    return 0;
}
