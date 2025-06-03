#include "game_state.c"
#include <ncurses.h>
#include <time.h>

int main(void) {
    // initialize ncurses
    initscr();
    cbreak();             // disable line buffering
    noecho();             // don't echo pressed keys
    keypad(stdscr, TRUE); // enable special keys

    srand(time(NULL));
    GameState *gs = GameState_create(4);

    // clear screen and print initial state
    clear();
    GameState_print(gs);
    printw("Press h/j/k/l to choose slide direction, q to quit:\n");
    refresh();

    int ch = 0;
    bool game_over = false;

    GameState *new_gs = NULL;
    while (!game_over && (ch = getch()) != 'q') {
        switch (ch) {
        case 'h':
            printw("slide left\n");
            new_gs = GameState_slide_and_merge_left(gs);
            break;
        case 'j':
            printw("slide down\n");
            new_gs = GameState_slide_and_merge_down(gs);
            break;
        case 'k':
            printw("slide up\n");
            new_gs = GameState_slide_and_merge_up(gs);
            break;
        case 'l':
            printw("slide right\n");
            new_gs = GameState_slide_and_merge_right(gs);
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
        printw("Press h/j/k/l to choose slide direction, q to quit:\n");
        refresh();
    }

    printw("Game Over!\nPress any key to quit...");
    refresh();
    getch(); // wait for final keypress

    // cleanup ncurses
    endwin();
    GameState_destroy(gs);
    return 0;
}
