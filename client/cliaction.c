#include "cliaction.h"

void htonl_arr(int * data,int len) {
    for(int i = 0; i < len; ++i)
        data[i]=htonl(data[i]);
}

void ntohl_arr(int * data,int len) {
    for(int i = 0; i < len; ++i)
        data[i]=ntohl(data[i]);
}

void status_mes(char *mes) {
    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    FILE *fptr = fopen("status.log", "a");
    char curTime[BUF_SIZE];
    strcpy(curTime, asctime(timeinfo));
    curTime[strlen(curTime) - 1] = 0;
    fprintf(fptr, "%s: %s\n", curTime, mes);
    fclose(fptr);
}

void error_mes(char *mes) {
    char err[BUF_SIZE];
    sprintf(err, "[Error] %s", mes) ;
    status_mes(err);
    exit(1);
}

void error_check(struct Header_Error_Res *header) {
    int errorCode = 0xF00;
    if((header->code & errorCode) == errorCode) {
        error_mes(header->errmsg);
    }
}


void send_data(int fd, struct Header_Base *header, size_t len) {
    struct Header_Base *data = calloc(1, len);
    memcpy(data, header, len);
    setLen(data->code, len);
    
    switch(header->code){
        case CMD_REGISTER: case CMD_JOIN: {
            htonl_arr((int*)(data), 8);
            break;
        }
        case CMD_START_GAME: case CMD_ROOM_INFO: case CMD_ACTION: 
        case CMD_MAP_INFO: case CMD_QUIT: case CMD_PLAYER_INFO: {
            htonl_arr((int*)(data), len / 4);
            break;
        }
        default: {
            free(data);
            error_mes("send_data error (not found header code)");
        }
    }

    status_mes("sending data");
    size_t re = write(fd ,data, len);
    free(data);
    
    if(re != len) {
        error_mes("send_data error (send length error)");
    }

    return;
}

void recv_data(int fd, struct Header_Base **header, size_t *len){
    struct Header_Base base;

    status_mes("reading header");
    int re = read(fd, &base, HEADER_SIZE);
    if(re != HEADER_SIZE) {
        error_mes("recv_data HEADER_SIZE error");
    }

    int dataLen = getLen(ntohl(base.code));

    struct Header_Base *data = calloc(1, dataLen);
    memcpy(data, &base, HEADER_SIZE);

    if(dataLen != HEADER_SIZE){
        status_mes("reading extra data");
        re = read(fd, ((void *)(data))+HEADER_SIZE, dataLen-HEADER_SIZE);
        if(re+HEADER_SIZE != dataLen) {
            free(data);
            error_mes("recv_data data length error");
        }
    }

    int code = getCode(ntohl(base.code));
    switch(code) {
        case RES_REGISTER_SUCC: case RES_JOIN_SUCC: 
        case RES_START_GAME_SUCC: case RES_ROOM_INFO_SUCC: {
            // return Header_Room_Info_Res
            ntohl_arr((int *)(data), 8);
            break;
        }
        case RES_ACTION_SUCC: case RES_MAP_INFO_SUCC: 
        case RES_PLAYER_INFO_SUCC: {
            ntohl_arr((int *)(data), dataLen / 4);
            break;
        }
        case RES_REGISTER_FAIL: case RES_JOIN_FAIL: case RES_START_GAME_FAIL:
        case RES_ROOM_INFO_FAIL: case RES_RECV_FAIL: case RES_ACTION_FAIL:
        case RES_MAP_INFO_FAIL: case RES_QUIT_FAIL: case RES_QUIT_SUCC: 
        case RES_PLAYER_INFO_FAIL: {
            ntohl_arr((int *)data, 8);
            break;
        }
        default:{
            free(data);
            error_mes("recv_data code error");
        }
    }
    clrLen(data->code);
    *header = data;
    *len = dataLen;

    error_check((struct Header_Error_Res*)(*header));
    return;
}

void connect_server(char *ip, char *port) {
    struct sockaddr_in servaddr;
    SOCKFD = socket(AF_INET, SOCK_STREAM, 0);
    memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(port));
	inet_pton(AF_INET, ip, &servaddr.sin_addr);
    status_mes("connecting to server");
	int n = connect(SOCKFD, (struct sockaddr *) &servaddr, sizeof(servaddr));
    if(n == -1) {
        error_mes("connect error");
    }
}


void room_register(int roomID, int playerNum, int szX, int szY,
    char *name, char *passwd) {
    status_mes("registering the room");
    struct Header_Room_Register myRegister;
    memset(&myRegister, 0, sizeof(myRegister));

    myRegister.code = CMD_REGISTER;
    myRegister.room_id = roomID;
    myRegister.player_number = playerNum;
    myRegister.sizeX = szX; 
    myRegister.sizeY = szY;
    strcpy(myRegister.name, name);
    strcpy(myRegister.passwd, passwd);

    struct Header_Room_Info_Res *res;
    size_t resLen;
    send_data(SOCKFD, (struct Header_Base*)(&myRegister), sizeof(myRegister));
    recv_data(SOCKFD, (struct Header_Base**)(&res), &resLen);

    SESSION = res->session; 
    ROOM_ID = res->room_id;
    PLAYER_ID = res->player_id;

    status_mes("finish room rigistered");
    return;
}

void room_join(int roomID, char *name, char *passwd) {
    status_mes("joining the room");

    struct Header_Room_Join request;
    struct Header_Room_Info_Res *result;
    size_t len;
    memset(&request, 0, sizeof(request));

    request.code = CMD_JOIN;
    request.room_id = roomID;
    strcpy(request.name, name);
    strcpy(request.passwd, passwd);
    send_data(SOCKFD, (struct Header_Base*)(&request), sizeof(request));
    recv_data(SOCKFD, (struct Header_Base**)(&result), &len);

    SESSION = result->session; 
    ROOM_ID = result->room_id;
    PLAYER_ID = result->player_id;

    status_mes("Finish room joined");
    return;
}

struct Header_Room_Info_Res* room_info() {
    status_mes("getting room info");

    if(!SOCKFD) {
        error_mes("not connect to server yet");
    }

    struct Header_Room_Info request;
    struct Header_Room_Info_Res *result;
    size_t len;
    memset(&request, 0, sizeof(request));

    request.code = CMD_ROOM_INFO;
    request.session = SESSION;
    request.room_id = ROOM_ID;
    request.player_id = PLAYER_ID;

    send_data(SOCKFD, (struct Header_Base*)(&request), sizeof(request));
    recv_data(SOCKFD, (struct Header_Base**)(&result), &len);

    status_mes("successfully get room info");
    return result;
}

void room_update() {
    status_mes("updating room info");

    struct Header_Room_Info_Res *room = room_info();

    PLAYER_NUMBER = room->player_number;
    PLAYER_CNT = 0;
    PLAYER_NAMES = calloc(PLAYER_NUMBER, 16);
    memcpy(PLAYER_NAMES, room->player_names, PLAYER_NUMBER*16);

    for(int i = 0; i < PLAYER_NUMBER; ++i) {
        if(room->player_names[i][0] != 0) {
            PLAYER_CNT++;
            for(int j = 0; j < 15; ++j) if(PLAYER_NAMES[i][j] == 0)
                PLAYER_NAMES[i][j] = ' ';
        }
    }

    SIZEX = room->sizeX;
    SIZEY = room->sizeY;

    GAME_STATE = room->game_state;

    free(room);

    status_mes("finish updating room info");
    return;
}

void room_quit() {
    status_mes("quiting the room");

    struct Header_Quit request;
    struct Header_Quit_Res *result;
    size_t len;
    memset(&request, 0, sizeof(request));

    request.code = CMD_QUIT;
    request.session = SESSION;
    request.room_id = ROOM_ID;
    request.player_id = PLAYER_ID;

    send_data(SOCKFD, (struct Header_Base*)(&request), sizeof(request));
    recv_data(SOCKFD, (struct Header_Base**)(&result), &len);

    status_mes("quit success");
    exit(0);
}

void start_game() {
    status_mes("request to start the game");

    struct Header_Start_Game request;    

    request.code = CMD_START_GAME;
    request.session = SESSION;
    request.room_id = ROOM_ID;
    request.player_id = PLAYER_ID;

    send_data(SOCKFD, (struct Header_Base*)(&request), sizeof(request));

    status_mes("successfully send the request");
    return;
}

void check_start_game() {
    status_mes("checking start_game request is received");

    struct Header_Room_Info_Res *result;
    size_t len;

    recv_data(SOCKFD, (struct Header_Base**)(&result), &len);
    if(result->code != RES_START_GAME_SUCC) {
        error_mes("fail to start game");
    }

    status_mes("can start the game now");
    return;
}

struct Header_Map_Info_Res* map_info() {
    status_mes("getting map info");

    struct Header_Map_Info request;
    struct Header_Map_Info_Res *result;
    size_t len;
    memset(&request, 0, sizeof(request));

    request.code = CMD_MAP_INFO;
    request.session = SESSION;
    request.room_id = ROOM_ID;
    request.player_id = PLAYER_ID;

    send_data(SOCKFD, (struct Header_Base*)(&request), sizeof(request));
    recv_data(SOCKFD, (struct Header_Base**)(&result), &len);

    status_mes("successfully get map info");
    return result;
}

void map_update() {
    status_mes("updating map info");

    struct Header_Map_Info_Res *map = map_info();

    SIZEX = map->sizeX;
    SIZEY = map->sizeY;

    ROUND = map->round;
    GAME_STATE = map->game_state;

    GRID = calloc(SIZEX * SIZEY, sizeof(struct Grid));
    memcpy(GRID, map->grid, SIZEX * SIZEY * sizeof(struct Grid));

    free(map);

    status_mes("finish updating map info");
    return;
}

struct Header_Player_Info_Res* player_info() {
    status_mes("getting player info");

    struct Header_Player_Info request;
    struct Header_Player_Info_Res *result;
    size_t len;
    memset(&request, 0, sizeof(request));

    request.code = CMD_PLAYER_INFO;
    request.session = SESSION;
    request.room_id = ROOM_ID;
    request.player_id = PLAYER_ID;

    send_data(SOCKFD, (struct Header_Base*)(&request), sizeof(request));
    recv_data(SOCKFD, (struct Header_Base**)(&result), &len);

    status_mes("successfully get player info");
    return result;

}

void player_update() {
    status_mes("updating player info");

    struct Header_Player_Info_Res *player = player_info();

    GAME_STATE = player->game_state;

    PLAYER_INFO = calloc(PLAYER_NUMBER, sizeof(struct Player_Info));
    memcpy(PLAYER_INFO, player->player, PLAYER_NUMBER * sizeof(struct Player_Info));

    free(player);

    status_mes("finish updating player info");
    return;
}

void postion_check() {
    if(POS_X < 0) POS_X = 0;
    if(POS_X >= SIZEX) POS_X = SIZEX - 1;
    if(POS_Y < 0) POS_Y = 0;
    if(POS_Y >= SIZEY) POS_Y = SIZEY - 1;
}

void action_requst(struct Action act) {
    status_mes("sending action request");

    struct Header_Action request;
    struct Header_Action_Res *result;
    size_t len;
    memset(&request, 0, sizeof(request));

    request.code = CMD_ACTION;
    request.session = SESSION;
    request.room_id = ROOM_ID;
    request.player_id = PLAYER_ID;

    request.num_Action = 1;
    request.action[0] = act;

    send_data(SOCKFD, (struct Header_Base*)(&request), sizeof(request));
    recv_data(SOCKFD, (struct Header_Base**)(&result), &len);

    status_mes("successfully send action request");
    return;
}

void action_init(){}

void action_get(char c) {
    status_mes("action_get");
    // q -> move
    // e -> select all
    // r -> select half;
    if(c == 'q')
        ACTION_MODE = ACTION_MODE_MOVE;
    else if(c == 'e')
        ACTION_MODE = ACTION_MODE_SELECT_ALL;
    else if(c == 'r')
        ACTION_MODE = ACTION_MODE_SELECT_HALF;
    else if(ACTION_MODE == ACTION_MODE_MOVE)
        action_move(c);
    else if(ACTION_MODE == ACTION_MODE_SELECT_ALL)
        action_select(c, 1);
    else if(ACTION_MODE == ACTION_MODE_SELECT_HALF)
        action_select(c, 0);
}

void action_move(char c) {
    status_mes("action_move");
    if(c == 'w')
        POS_X--;
    if(c == 'a')
        POS_Y--;
    if(c == 's')
        POS_X++;
    if(c == 'd')
        POS_Y++;
    postion_check();
}

void action_select(char c, int mode) {
    status_mes("action_selcet");
    int way = 0;

    if(c == 'w')
        way = 2;
    if(c == 'a')
        way = 3;
    if(c == 's')
        way = 0;
    if(c == 'd')
        way = 1;

    struct Action act;
    act.x = POS_X;
    act.y = POS_Y;
    act.way = way;
    act.all_or_half = (mode == ACTION_MODE_SELECT_ALL ? 0 : 1); // ???????? 

    action_requst(act);
    action_move(c);
}