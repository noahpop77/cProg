/*
============
NOTES
============

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


    The elements of the struct are bitmasks. Each
    bit in the bitmask is a configurable flag which
    can be set.
    
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

enterCustomTermMode() ---------------------------------------------

Read one character at a time
VMIN and VTIME are special non canonical flags
VMIN = characters per non-canonical read
VTIME = time delay in deciseconds between reads

https://man7.org/linux/man-pages/man3/termios.3.html
     ctrl + f for cfmakeraw

rawTerm.c_lflag &= ~(ICANON | ECHO);

Replaces ^^^^^^^^^^^^^^^^^^^^^^^^^^^ with cfmakeraw(). 
Doing it with cfmakeraw is the function supported way of 
putting a terminal in raw mode and the above method is 
the manual way of altering the bitmask to do so.

printBox(int x) ------------------------------------------------------

Repeats the printing of a colored box horizontally for X amount of cells

You can replace the empty space in the print with any text to color it
printf("\033[41m \033[0m")
                ^

*/


#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>

static int termWidth;
static int termHeight;

void getTermSize() {

    struct winsize termSize;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &termSize);
    termWidth = termSize.ws_col;
    termHeight = termSize.ws_row;
}

struct termios enterCustomTermMode () {

    getTermSize();

    printf("\x1b[?1049h\x1b[H"); // Enter alt mode
    fflush(stdout);
    
    // Enter raw mode
    struct termios originalTerm, rawTerm;
    tcgetattr(STDIN_FILENO, &originalTerm);
    

    rawTerm = originalTerm;
    cfmakeraw(&rawTerm);


    rawTerm.c_cc[VMIN] = 1;
    rawTerm.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSANOW, &rawTerm);

    return originalTerm; // Pass state of terminal to use in exitCustomTermMode
}

void exitCustomTermMode (struct termios orig_termios) {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios); // Rvrt trm 2 org

    printf("\x1b[?1049l"); // Leave alt screen buffer
    fflush(stdout);
}


void printBox(int cells) {
    for (int i = 0; i < cells ; i ++) {
        printf("\033[41m \033[0m");
    }
    printf("\r\n");
}

int main(void)
{

    // -------------------------------------------------
    struct termios termSettings = enterCustomTermMode();

    printBox(termWidth);

    printf("STDIN_FILENO\t(%d)\r\n", STDIN_FILENO);
    printf("STDOUT_FILENO\t(%d)\r\n", STDOUT_FILENO);
    printf("STDERR_FILENO\t(%d)\r\n", STDERR_FILENO);

    printf("Terminal Width = %d\r\n", termWidth);
    
    printf("Welcome to Alt Mode!\r\n");
    printf("Press q to quit...\r\n");
    
    printBox(termWidth);
    fflush(stdout);

    char key;
    while ((key = getchar()) != 'q'){}

    
    exitCustomTermMode(termSettings);
    // -------------------------------------------------

    return 0;

}
