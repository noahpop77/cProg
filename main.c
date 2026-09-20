#include <stdio.h>
#include <unistd.h>
#include <termios.h>

struct termios enterCustomTermMode () {
    printf("\x1b[?1049h\x1b[H");
    fflush(stdout);

    struct termios orig_termios, raw_termios;
    tcgetattr(STDIN_FILENO, &orig_termios);

    return orig_termios;
}

void exitCustomTermMode (struct termios orig_termios) {
    // 3. Clean up: Restore original termios attributes
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);

    // 4. Leave alternate screen buffer
    printf("\x1b[?1049l");
    fflush(stdout);
}

int main(void)
{

    struct termios termSettings = enterCustomTermMode();
    
    // ---- Your UI / Game Loop / Program goes here ----
    printf("STD_FILENO is an int with value: %d\n", STDIN_FILENO);
    printf("Welcome to Alt Mode!\n");
    printf("Press any key to exit and restore your normal terminal...");
    fflush(stdout);

    getchar(); 
    // -------------------------------------------------
    
    exitCustomTermMode(termSettings);

    return 0;

}
