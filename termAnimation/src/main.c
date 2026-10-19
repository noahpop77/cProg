#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <stdlib.h>

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

int main()
{
    signal(SIGINT, handle_sigint);

    struct TermSize termSize = {0};
    termSize = setTermSize(termSize);
    termSettings = enterCustomTermMode();
    
    // Struct used to track/manipulate cursor position easily
    struct curPo posi = { 1, 1 };
    int counter = 0;

    for (;;){
        clearScreen();
        // \033[%d;%dH - First %d is row, Second is column
        printf("\033[%d;%dH", posi.row, posi.col);
        printf("\033[41m          \033[0m");

        posi.col++; // Moves the cursor along to the right
                    //
        // Makes it so that it will move the cursor down 3 rows
        // every 5 iterations and will not make it go down on
        // the first iteration hence the && ....
        if (counter % 5 == 0 && counter >= 5){
            posi.row = posi.row + 3;
        }

        counter++;
        fflush(stdout);
        usleep(500000);
    }
    // Awaits Q to exit program
    char key;
    while ((key = getchar()) != 'q'){}
    exitCustomTermMode(termSettings);

    return 0;

}
