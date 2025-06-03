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
    printw("                                            w        ↑   \n");
    printw("Choose slide direction with:   h j k l,   a s d,   ← ↓ →.\n");
    printw("\nUndo with:                     u, z, space.\n");
    refresh();

    int ch = 0;
    bool game_over = false;

    GameState *new_gs = NULL;
    while (!game_over && (ch = getch()) != 'q') {
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
            break;
        default:
            printw("unknown key '%c'\n", ch);
            break;
        }

        // if change occured
        if (new_gs) {
            game_over =
                !GameState_add_random(new_gs) || !GameState_can_move(new_gs);
            gs = new_gs;
        }

        // clear screen and redraw
        clear();
        GameState_print(gs);
        refresh();
    }

    printw("Game Over!\nPress any key to quit...");
    refresh();
    getch(); // wait for final keypress

    // cleanup ncurses
    endwin();
    GameState_destroy_chain(gs);
    return 0;
}
