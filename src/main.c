#include <stdio.h>

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
