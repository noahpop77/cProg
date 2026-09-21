#include <stdio.h>
#include <unistd.h>
#include <termios.h>

/*
Pre Processor Macros-----------------------------
    Pre Processor macros included with <unistd.h>
    
    STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO

    Effectively expand any instance of the macro
    to what it was defined as, in this case it
    expands to 0, 1, 2 respectively

Struct termios structure-------------------------

    The tcgetattr and tcsetattr methods get and
    a desired configuration for the terminal.
    
    You can get the existing config, change it
    during runtime and revert back to the
    previous state using get -> set.

    State (the orig_termios) needs to be passed
    from the get to the set methods so we can
    revert safely and accurately after the program
    is terminated.

int     tcgetattr(int, struct termios *);
int     tcsetattr(int, int, const struct termios *);

struct termios {
	tcflag_t        c_iflag;      //  input flags
	tcflag_t        c_oflag;      //  output flags
	tcflag_t        c_cflag;      //  control flags
	tcflag_t        c_lflag;      //  local flags
	cc_t            c_cc[NCCS];   //  control chars
	speed_t         c_ispeed;     //  input speed
	speed_t         c_ospeed;     //  output speed
};

ANSII codes--------------------------------------


    Code to enter alt mode
        \x1b[?1049h\x1b[H
    Code to exit alt mode
        \x1b[?1049l
    Color a character red (in this case an empty space)
        \033[41m \033[0m
*/


struct termios enterCustomTermMode () {
    // Enter alt mode
    printf("\x1b[?1049h\x1b[H");
    fflush(stdout);
    
    // Enter raw mode
    struct termios originalTerm, rawTerm;
    tcgetattr(STDIN_FILENO, &originalTerm);

    rawTerm.c_lflag &= ~(ICANON | ECHO);

    // Read one character at a time
    rawTerm.c_cc[VMIN] = 1;
    rawTerm.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &rawTerm);

    // Has to pass state of terminal to use in exitCustomTermMode
    return originalTerm;
}

void exitCustomTermMode (struct termios orig_termios) {
    // Restore original termios attributes
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);

    // Leave alt screen buffer
    printf("\x1b[?1049l");
    fflush(stdout);
}

// Repeats the printing of a colored box horizontally for X amount of cells
//      You can replace the empty space in the print with any text to color it
void printBox(int cells) {
    for (int i = 0; i < cells ; i ++) {
        printf("\033[41m \033[0m");
    }
    printf("\r\n");
}

int main(void)
{
    struct termios termSettings = enterCustomTermMode();

    // ---- Term alt screen code goes here ----
    printf("STDIN_FILENO is an int with value: %d\r\n", STDIN_FILENO);
    printf("STDOUT_FILENO is an int with value: %d\r\n", STDOUT_FILENO);
    printf("STDERR_FILENO is an int with value: %d\r\n", STDERR_FILENO);

    printBox(20);

    printf("Welcome to Alt Mode!\r\n");
    printf("Press enter to quit...\r\n");
    fflush(stdout);

    //getchar(); 
    
    char key;
    while ((key = getchar()) != 'q'){}

    // -------------------------------------------------
    
    exitCustomTermMode(termSettings);

    return 0;

}
