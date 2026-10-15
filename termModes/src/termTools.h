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


struct TermSize {
    int termWidth;
    int termHeight;
};

int strcmpr(const char *first, const char *second);
int stringLength(const char *input_string);
struct TermSize setTermSize(struct TermSize termSize);
struct termios enterCustomTermMode ();
void exitCustomTermMode (struct termios orig_termios);
void printBar(int cells);
void printBarRange(int cells);
void printBarrage(const char *in_str, int count);
void printBox(char *input_string);
int  cliFlagChecker(int argc, char **argv);
void printCenterText(const char *input_string);
void titlecard(int argc, char **argv);

#ifndef TERM_HELPER
#define TERM_HELPER

#ifdef TERM_IMPL

int strcmpr(const char *first, const char *second) {
    while (*first == *second) {
        // if (*first != '\0' && *second != '\0') {
        if (*first == '\0') {
            return 1;
        }
        first++;
        second++;
    }
    return 0;
}

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

void printBarRange(int cells) {
    for (int i = 0; i < cells ; i ++) {
        printf("\033[41m \033[0m");
    }
    printf("\r\n");
}

void printBarrage(const char *in_str, int count) {
    for (int i = 0; i < count; i++) {
        printf("%s", in_str);
    }
}

void titlecard(int argc, char **argv) {
    struct TermSize termSize = {0};
    termSize = setTermSize(termSize);
    int x = termSize.termWidth/2;
    int x_shift = termSize.termWidth/4;
    int y_shift = termSize.termHeight / 3;
    
    for (int i = 0; i < y_shift; i++) {
        printf("\r\n");
    }
    for (int i = 0; i < x_shift; i++) {
        printf(" ");
    }
    printBarRange(x);
    printf("\r\n");

    for (int i = 0; i < argc; i++) {
        if (strcmpr(argv[i], "-titlecard")) {
            for (int title = i + 1; title < argc; title++) {
                printCenterText(argv[title]);
            }
        }
    }

    printf("\r\n");
    for (int i = 0; i < x_shift; i++) {
        printf(" ");
    }
    printBarRange(x);
    for (int i = 0; i <= x_shift-1; i++) {
        printf(" ");
    }
    printf("\r\n");
    
}


// BOX CODE HEREEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE
void printBox(char *input_string) {
    struct TermSize termSize = {0};
    termSize = setTermSize(termSize);
    int input_length = stringLength(input_string);
    int x = termSize.termWidth/2;
    int x_shift = termSize.termWidth/4;
    int y = termSize.termHeight/2;
    int y_shift = termSize.termHeight / 4;
    
    for (int i = 0; i < y_shift; i++) {
        printf("\r\n");
    }
    for (int i = 0; i < x_shift; i++) {
        printf(" ");
    }
    printBarRange(x);
    // for (int i = 0; i <= x_shift-1; i++) {
    //     printf(" ");
    // }
    // printf("\r\n");

    for (int i = 0; i < y; i++) {
        
        // Handles case of printing the actual line with the text in it
        if (i == y_shift){
            printBarrage(" ", x_shift);
            printf("\033[41m  \033[0m");

            // For odd length input_string we rmv 1 extra " " from start
            if ((input_length) % 2 == 1) {
                printBarrage(" ", x_shift - input_length/2 - 3);
            } else {
                printBarrage(" ", x_shift - input_length/2 - 2);
            }
            printf("%s", input_string);
            printBarrage(" ", x_shift - input_length/2 - 2); 
            printf("\033[41m  \033[0m");
            printBarrage(" ", x_shift);
            printf("\r\n");
            continue;
        }

        // Handles printing inner box content with 
        // width pad < > box wall < > inner pad <> box wall < > width pad
        for (int i = 0; i < termSize.termWidth-2; i++) {
            
            // Mod calc to make square consistent acrosss trm sizes            
            if (x % x_shift == 1 ){
                if (i == x_shift || i == (x_shift*3) - 2){
                    printf("\033[41m  \033[0m");
                    continue;
                }
            }
            else {
                if (i == x_shift || i == (x_shift*3) - 3){
                    printf("\033[41m  \033[0m");
                    continue;
                }
            }
            printf(" ");
        }
    }

    printf("\r\n");
    for (int i = 0; i < x_shift; i++) {
        printf(" ");
    }
    printBarRange(x);
    // for (int i = 0; i <= x_shift-1; i++) {
    //     printf(" ");
    // }
    // printf("\r\n");
    
}

// Box Handler
// 3 code paths---------------------
// 1. -box is there, text is valid
// 2. -box is there, text isnt valid
// 3. no -box
int cliFlagChecker(int argc, char **argv) {
    for (int i = 0; i < argc; i++) {
        if (strcmpr(argv[i], "-box")) {
            if ((i+1) >= argc) {
                // 2
                return 1;
            }
            else {
                // TODO: BOX FUNCTIONALITY HERE IN printBox
                printBox(argv[i+1]);
                // 1
                return 2;
            }
        } else if (strcmpr(argv[i], "-titlecard")) {
            if ((i+1) >= argc) {
                // 2
                return 1;
            }
            else {
                titlecard(argc, argv);
                // 1
                return 2;
            }
        }
    }
    // 3
    return 0;
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

#endif
