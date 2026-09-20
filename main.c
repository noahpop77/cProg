#include <stdio.h>
#include <unistd.h>
#include <termios.h>

struct termios enterCustomTermMode () {
    // Enter alt mode
    printf("\x1b[?1049h\x1b[H");
    fflush(stdout);
    
    // Enter raw mode
    struct termios orig_termios, raw_termios;
    tcgetattr(STDIN_FILENO, &orig_termios);
    
    // Has to pass state of terminal to use in exitCustomTermMode
    return orig_termios;
}

void exitCustomTermMode (struct termios orig_termios) {
    // Restore original termios attributes
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);

    // Leave alt screen buffer
    printf("\x1b[?1049l");
    fflush(stdout);
}

void printBox() {
    for (int i = 0; i < 50 ; i ++) {
        printf("\033[41m \033[0m");
    }
    printf("\n");
}

int main(void)
{
    struct termios termSettings = enterCustomTermMode();

    // ---- Your UI / Game Loop / Program goes here ----
    printf("STD_FILENO is an int with value: %d\n", STDIN_FILENO);
    printf("STD_FILENO is an int with value: %d\n", STDOUT_FILENO);
    printf("STD_FILENO is an int with value: %d\n", STDERR_FILENO);

    printf("\033[31mTheWord\033[0m\n"); // Adds an alt code to text on the inside

    printBox();

    printf("Welcome to Alt Mode!\n");
    printf("Press enter to quit...");
    fflush(stdout);

    getchar(); 
    // -------------------------------------------------
    
    exitCustomTermMode(termSettings);

    return 0;

}
