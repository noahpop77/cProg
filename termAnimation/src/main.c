#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>

#define TERM_IMPL
#include "termTools.h"

static struct termios termSettings;

// Signal handler for Ctrl + C (SIGINT)
void handle_sigint(int sig) {
    
    // Silence the "unused parameter" warning by casting sig to void
    (void)sig; 
    
    exitCustomTermMode(termSettings);

    // Resets terminal colors and default cursor behavior
    printf("\033[0m\033[?25h"); 
    
    printf("\nProgram interrupted. Exited cleanly.\n");
    exit(0);
}

void clearScreen(){
    printf("\033[2J");
}

struct curPo {
  int row;
  int col;
};
// --snake | SNAKE LOCATION CODE HERE ==========================================
void snake(){
    struct curPo posi = { 1, 1 };
    struct TermSize termSize = {0};
    termSize = setTermSize(termSize);

    for (;;){
        clearScreen();
        // \033[%d;%dH - First %d is row, Second is column
        printf("\033[%d;%dH", posi.row, posi.col);
        printf("\033[101m          \033[0m");

        posi.col++; // Moves the cursor along to the right

        if (posi.col == termSize.termWidth - 8){
            posi.col = 1;   // Resets col when it reaches the right end of term
            posi.row++;
        }
        if (posi.row == termSize.termHeight + 1){
            posi.row = 1;   // Resets row when it reaches bottom of term
        }

        fflush(stdout);
        usleep(8000);
    }

}
// --random | RANDOM LOCATION CODE HERE ========================================
int randomNum(int min, int max){
    srand(time(NULL));
    return (rand() % (max - min + 1)) + min;
}

void randomLocation(){
    srand(time(NULL));
    struct curPo posi = { 1, 1 };

    struct TermSize termSize = {0};
    termSize = setTermSize(termSize);

    for (;;){
        //clearScreen();
        int randRow = (rand() % termSize.termHeight) + 1;
        int maxCol = termSize.termWidth - 1;
        int randCol = (rand() % maxCol) + 1;

        posi.row = randRow;
        posi.col = randCol - 2;
        printf("\033[%d;%dH", posi.row, posi.col);
        printf("\033[101m  \033[0m");   // 10 characters wide

        fflush(stdout);
        usleep(8000);
    }

}


void cliFlags(int argc, char **argv){
    for (int i = 0; i < argc; i++) {
        if (strcmpr(argv[i], "--random")) {
            randomLocation();
        } else if (strcmpr(argv[i], "--snake")) {
            snake();
        } else if (argc == 1 || strcmpr(argv[i], "--help")) {
            printf("-- Help Menu ---\r\n");  
            printf("-random\r\n");
            printf("\tPrints to screen at random locations\r\n");  
            printf("-snake\r\n");  
            printf("\tSnakes across the screen from left to right, up to down\r\n");  
        }
    }
}


int main(int argc, char **argv)
{
    signal(SIGINT, handle_sigint);
    
    termSettings = enterCustomTermMode();
    
    cliFlags(argc, argv);

    // Awaits Q to exit program
    // char key;
    // while ((key = getchar()) != 'q'){}
    // exitCustomTermMode(termSettings);

    return 0;

}
