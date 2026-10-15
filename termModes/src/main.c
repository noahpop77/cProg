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

    The below pre processor macros are just constants
    that are used to specify which file descriptor
    you want to write to.

STDIN_FILENO  = 0
STDOUT_FILENO = 1
STDERR_FILENO = 2

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

printBox(int x) ------------------------------------------------------1 2 3 4 5 6 7 8

Repeats the printing of a colored box horizontally for X amount of cells

You can replace the empty space in the print with any text to color it
printf("\033[41m \033[0m")
                ^

*/

// FUN implement strcmp from scratch with pointer arith

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>

#define TERM_IMPL
#include "termTools.h"

// Main Entrypoint of code
int main(int argc, char **argv)
{


    // long double sum = 0;
    // for (int i = 1; i <= 828000; i++) {
    //     long double odd_num = 2 * i - 1;
    //     sum += odd_num * odd_num;
    // }
    //
    // printf("%Lf\r\n", sum);

    uint64_t N = 828000;
    uint64_t testn = 125000;
    uint64_t x = testn / 2;
    // Exactly half of the integers from 1 to N are odd
    uint64_t k = N / 2;
    
    // (4k^3 + 6k^2 + 2k) / 3
    // Equivilent: 2k(k+1)(2k+1) / 3
    uint64_t my_sum = ((4*x*x*x) + (6*x*x) + (2*x))/3;
    // Formula for sum of first k odd squares: k * (4 * k^2 - 1) / 3
    uint64_t total_sum = k * (4 * k * k - 1) / 3;
    
    // %glu for unsigned 64-bit integers (PRIu64 format)
    printf("Sum of odd squares: %llu\r\n", (unsigned long long)total_sum);
    printf("Sum of even squares till 125k: %llu\r\n", (unsigned long long)my_sum);




    // Program wide terminal size struct, no global vars
    struct TermSize termSize = {0};
    termSize = setTermSize(termSize);

    // Enter alt mode
    struct termios termSettings = enterCustomTermMode();
    
    int cliStatus = cliFlagChecker(argc, argv);
    if (cliStatus == 1) {
        exitCustomTermMode(termSettings);
        printf("No valid input for -box provided\r\n");
        return 1;
    } else if (cliStatus == 2) { 
        char key;
        while ((key = getchar()) != 'q'){}
        exitCustomTermMode(termSettings);
        return 0;
    }

    printBar(termSize.termWidth);
    printCenterText("BENOS");
    printCenterText("Welcome to ALT mode!");

    for (int i = 0; i < argc; i++) { printf("%s\r\n", argv[i]); }

    printf("Press q to quit...\r\n");
    
    printBar(termSize.termWidth);
    fflush(stdout);
    
    // Awaits Q to exit program
    char key;
    while ((key = getchar()) != 'q'){}
    
    exitCustomTermMode(termSettings);

    return 0;

}
