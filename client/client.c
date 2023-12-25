#include "cliaction.h"
#include "cliscreen.h"
#include "clistate.h" 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <termios.h>


void connect_screen() {
    // Connect to Server
    clear_screen();

    char ip[BUF_SIZE], buf[BUF_SIZE];
    int port;
    print_at("Enter Server IP:", 1, 1);
    read_at(ip, 1, 17);
    print_at("Enter Port Number:", 2, 1);
    read_at(buf, 2, 19);
    port = atoi(buf);

    struct sockaddr_in servaddr;
    SOCKFD = socket(AF_INET, SOCK_STREAM, 0);
    memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	inet_pton(AF_INET, ip, &servaddr.sin_addr);
    int n;
	if ((n = connect(SOCKFD, (struct sockaddr *) &servaddr, sizeof(servaddr))) == 0) {
        // success
        print_loading("Connecing ", 3, 1);
    }
    else {
        print_at("BAD", 3, 1);
        exit(1);
    }

    fflush(stdout);
    sleep(1);
}

void loading_screen() {
    // Let user can go into full screen

    char str[] = "Enter Full Screen Mode to get better experience";
    char waitres[] = "Press any key to continue ...";
    char loading[] = "Loading...";
    clear_screen();
    print_at(str, 1, 1);
    print_at(waitres, 2, 1);
    fflush(stdout);

    disable_cbreak();
    fgetc(stdin);
    fgetc(stdin);
    enable_cbreak();

    clear_line(2);
    print_at_slow(loading, 2, 1);

    sleep(1);
}

void entry_screen() {
    // game entry
    clear_screen();

    int h, w;
    get_screen_size(&h, &w);
    int hh = (h / 4 <= 0 ? 0 : h / 4);
    int ww = (w / 2 - 40 <= 0 ? 1 : w / 2 - 40);

    char name[BUF_SIZE];
    char isNew[BUF_SIZE];
    char buf[BUF_SIZE];
    int roomID, playerNum;
    int sizeX, sizeY, passwd;


    print_at(" ██████╗     ██████╗ ███████╗███╗   ██╗███████╗██████╗  █████╗ ██╗     ███████╗", hh, ww);
    print_at("██╔════╝    ██╔════╝ ██╔════╝████╗  ██║██╔════╝██╔══██╗██╔══██╗██║     ██╔════╝", hh + 1, ww);
    print_at("██║         ██║  ███╗█████╗  ██╔██╗ ██║█████╗  ██████╔╝███████║██║     ███████╗", hh + 2, ww);
    print_at("██║         ██║   ██║██╔══╝  ██║╚██╗██║██╔══╝  ██╔══██╗██╔══██║██║     ╚════██║", hh + 3, ww);
    print_at("╚██████╗    ╚██████╔╝███████╗██║ ╚████║███████╗██║  ██║██║  ██║███████╗███████║", hh + 4, ww);
    print_at(" ╚═════╝     ╚═════╝ ╚══════╝╚═╝  ╚═══╝╚══════╝╚═╝  ╚═╝╚═╝  ╚═╝╚══════╝╚══════╝", hh + 5, ww);
    print_at("                   Created By: freddy645645, benson0402", hh + 6, ww);
    // hh + 7
    // hh + 8
    print_at("                        Enter Your Name:", hh + 9, ww);
    read_at(name, hh + 9, ww + 41);
    print_at("                        Create Room [Y/N]:", hh + 10, ww);
    read_at(isNew, hh + 10, ww + 43);

    // create new room
    if(!strcmp(isNew, "Y")) {

    print_at("                        Enter Your Room ID:", hh + 11, ww);
        read_at(buf, hh + 11, ww + 44);
        roomID = atoi(buf);
    print_at("                        Enter Room Password:", hh + 12, ww);
        read_at(buf, hh + 12, ww + 45);
        passwd = atoi(buf);
    print_at("                        Enter Max Player Number:", hh + 13, ww);
        read_at(buf, hh + 13, ww + 49);
        playerNum = atoi(buf);
    print_at("                        Enter Map Max X-axis:", hh + 14, ww);
        read_at(buf, hh + 14, ww + 46);
        sizeX = atoi(buf);
    print_at("                        Enter Map Max Y-axis:", hh + 15, ww);
        read_at(buf, hh + 15, ww + 46);
        sizeY = atoi(buf);

        // Register 

    }
    else {

    print_at("                        Enter Your Room ID:", hh + 11, ww);
        read_at(buf, hh + 11, ww + 44);
        roomID = atoi(buf);
    print_at("                        Enter Room Password:", hh + 12, ww);
        read_at(buf, hh + 12, ww + 45);
        passwd = atoi(buf);

        // Join
    }
    
    fflush(stdout);
    sleep(1);
}


int main(int argc, char **argv) {
    cursor_visable(0);
    disable_echo();

    connect_screen();
    loading_screen();
    entry_screen();

    cursor_visable(1);
    enable_echo();
}