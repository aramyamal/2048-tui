#include "uint16array.c"
#include <stdio.h>

typedef struct {
    UInt16Array tiles;
    UInt16Array previous;
    size_t dimension;
    size_t prev_left;
    int32_t score;
} GameState;

GameState GameState_Init(size_t dimension) {
    UInt16Array tiles = UInt16Array_Init(dimension, dimension);
    return (GameState){
        .tiles = tiles,
        .previous = tiles,
        .dimension = dimension,
        .prev_left = 3,
        .score = 0,
    };
}

int main(void) {
    printf("Press h/j/k/l to choose slide direction, q to quit:\n");
    int ch;
    while ((ch = getchar()) != 'q') {
        switch (ch) {
        case 'h':
            printf("slide left\n");
            break;
        case 'j':
            printf("slide down\n");
            break;
        case 'k':
            printf("slide up\n");
            break;
        case 'l':
            printf("slide right\n");
            break;
        default:
            printf("unknown key '%c'\n", ch);
            break;
        }
        while (ch != '\n' && ch != EOF)
            ch = getchar();
    }
    return 0;
}
