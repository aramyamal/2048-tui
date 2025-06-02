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

    while ((ch = getch()) != 'q' && !game_over) {
        switch (ch) {
        case 'h':
            printw("slide left\n");
            gs = GameState_slide_and_merge_left(gs);
            break;
        case 'j':
            printw("slide down\n");
            gs = GameState_slide_and_merge_down(gs);
            break;
        case 'k':
            printw("slide up\n");
            gs = GameState_slide_and_merge_up(gs);
            break;
        case 'l':
            printw("slide right\n");
            gs = GameState_slide_and_merge_right(gs);
            break;
        default:
            printw("unknown key '%c'\n", ch);
            break;
        }
        game_over = !GameState_add_random(gs);

        // clear screen and redraw
        clear();
        GameState_print(gs);
        printw("Press h/j/k/l to choose slide direction, q to quit:\n");
        refresh();
    }

    printw("Game Over!\nPress any key to quite...");
    refresh();
    getch(); // wait for final keypress

    // cleanup ncurses
    endwin();
    GameState_destroy(gs);
    return 0;
}
