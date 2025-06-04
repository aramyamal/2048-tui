#include "game_state.c"
#include "render.c"
#include <locale.h>
#include <ncurses.h>
#include <stdint.h>
#include <time.h>

#define DIMENSION 4
#define UNDOS 3

int main(void) {
    // set locale for unicode support
    setlocale(LC_ALL, "");

    // initialize ncurses
    initscr();
    cbreak();             // disable line buffering
    noecho();             // don't echo pressed keys
    keypad(stdscr, TRUE); // enable special keys

    srand(time(NULL));
    GameState *gs = GameState_create(DIMENSION, UNDOS);

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

    int ch = 0;
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
