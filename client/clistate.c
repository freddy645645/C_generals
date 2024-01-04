#include "clistate.h"

void state_connect() {
    clear_screen();

    char ip[BUF_SIZE], port[BUF_SIZE];
    print_at("Enter Server IP:", 1, 1);
    read_at(ip, 1, 17);
    print_at("Enter Port Number:", 2, 1);
    read_at(port, 2, 19);

    connect_server(ip, port);
    print_loading("Connecing ", 3, 1);

    fflush(stdout);
    sleep(1);
}

void state_loading() {
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

void state_entry() {
    clear_screen();

    int h, w;
    get_screen_size(&h, &w);
    int hh = (h / 4 <= 0 ? 0 : h / 4);
    int ww = (w / 2 - 40 <= 0 ? 1 : w / 2 - 40);

    char name[BUF_SIZE];
    char passwd[BUF_SIZE];
    char isNew[BUF_SIZE];
    char buf[BUF_SIZE];
    int roomID, playerNum;
    int sizeX, sizeY;


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
        memcpy(buf, passwd, BUF_SIZE);
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
        room_register(roomID, playerNum, sizeX, sizeY, name, passwd);
    }
    else {

    print_at("                        Enter Your Room ID:", hh + 11, ww);
        read_at(buf, hh + 11, ww + 44);
        roomID = atoi(buf);
    print_at("                        Enter Room Password:", hh + 12, ww);
        read_at(buf, hh + 12, ww + 45);
        memcpy(buf, passwd, BUF_SIZE);

        // Join
        room_join(roomID, name, passwd);
    }
    
    fflush(stdout);
    sleep(1);
}

void state_room() {
    while(1) {
        clear_screen();
        room_update();

        int h = 1, colW = 20;
        char str[BUF_SIZE];
        sprintf(str, "Room ID: %d\n", ROOM_ID);
        print_middle(str, h);
        sprintf(str, "Current Wating Player Number: %d/%d\n", PLAYER_CNT, PLAYER_NUMBER);
        print_middle(str, h + 1);
        sprintf(str, "%-*s | %-*s\n", colW, "Player Index", colW, "Player Name");
        print_middle(str, h + 3);
        for(int i = 0; i < PLAYER_NUMBER; ++i) {
            sprintf(str, "%-*d | %-*s\n", colW, i, colW, PLAYER_NAMES[i]);
            print_middle(str, h + 4 + i);
        }
        sprintf(str, "Enter 'q' to leave or 'r' to force the game start: ");
        int tail = print_middle(str, h + 5 + PLAYER_NUMBER);
        fflush(stdout);

        enable_echo();
        if(read_at_wait(str, h + 5 + PLAYER_NUMBER, tail, 1, 0)) {
            disable_echo();
            if(!strcmp(str, "q")) {
                room_quit();
            }
            else if(!strcmp(str, "r")) {
                start_game();
                check_start_game();
                break;
            }
        }
        disable_echo();
        if(read_server_check(0, 500000)) {
            check_start_game();
            break;
        }

        fflush(stdout);
        usleep(500000);
    } 
}

void state_game() {
    clear_screen();
    // print_middle("IN GAME", 2);

    map_update();
    player_update();

    int h = 1, colW = 13;
    char str[BUF_SIZE];
    sprintf(str, "Room ID: %d\n", ROOM_ID);
    print_middle(str, h);
    sprintf(str, "%-*s | %-*s | %-*s | %-*s | %-*s \n", colW, "Player Index", colW, "Player Name",
        colW, "Grid Num", colW, "Soldier Num", colW, "Player state");
    print_middle(str, h + 3);
    for(int i = 0; i < PLAYER_NUMBER; ++i) {
        sprintf(str, "%-*d | %-*s | %-*d | %-*d | %-*d\n", colW, i, colW, PLAYER_NAMES[i],
            colW, PLAYER_INFO[i].grid_num, colW, PLAYER_INFO[i].soldier_num,
            colW, PLAYER_INFO[i].player_state);
        print_middle(str, h + 4 + i);
    }

    fflush(stdout);
    sleep(3);
}