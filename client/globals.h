#ifndef GLOBALS_INCLUDED
#define GLOBALS_INCLUDED

#include "data_format.h"

#define BUF_SIZE 1024

extern int SOCKFD;
extern int SESSION, ROOM_ID, PLAYER_ID;

extern int PLAYER_NUMBER;
extern int PLAYER_CNT;
extern char (*PLAYER_NAMES)[16];

extern int SIZEX, SIZEY;

extern int GAME_STATE;

extern int ROUND;

extern struct Grid *GRID;

extern struct Player_Info *PLAYER_INFO;

#endif