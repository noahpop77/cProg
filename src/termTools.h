/*
============
NOTES
============

Custom strlen Implementation -----------------------

    It uses pointer Arithmatic to get the legnth 
    of string. *ptr is the [current location] and 
    *input_string is the [starting location].
    
    We take the dif of the addresses once we find
    the null terminator [\0] and the dif is the length

int stringLength(const char *input_string)

*/

#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>

#ifdef LIBRARY_IMPLEMENTATION

struct TermSize {
    int termWidth;
    int termHeight;
};

int stringLength(const char *input_string) {
    const char *ptr = input_string;
    while (*ptr != '\0') {
        ptr++;
    }
    int input_length = ptr - input_string;
    return input_length;
}

struct TermSize setTermSize(struct TermSize termSize) {

    struct winsize winSize;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &winSize);
    termSize.termWidth = winSize.ws_col;
    termSize.termHeight = winSize.ws_row;
    return termSize;
}

struct termios enterCustomTermMode () {

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


void printBar(int cells) {
    for (int i = 0; i < cells ; i ++) {
        printf("\033[41m \033[0m");
    }
    printf("\r\n");
}



// TODO: Add a way to do things like ("%d", myInt)
void printCenterText(const char *input_string) {

    struct TermSize termSize = {0};
    termSize = setTermSize(termSize);
    int input_length = stringLength(input_string);

    int padding = (termSize.termWidth - input_length) / 2;

    for (int i = 0; i < padding; i++) {
        printf(" ");
    }

    // YOUR TEXT GOES HERE
    printf("%s", input_string);

    for (int i = 0; i < padding; i++) {
        printf(" ");
    }
    printf("\r\n");
}
#endif
