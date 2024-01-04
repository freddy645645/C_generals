#include "cliaction.h"
#include "cliscreen.h"
#include "clistate.h" 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <termios.h>


void gameflow() {
    cursor_visable(0);
    disable_echo();

    state_connect();
    state_loading();
    state_entry();
    state_room();
    state_game();

    cursor_visable(1);
    enable_echo();
}

int main(int argc, char **argv) {
    gameflow();    
}