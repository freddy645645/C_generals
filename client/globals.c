#include "globals.h"

int SOCKFD = 0;
int SESSION = 0, ROOM_ID = 0, PLAYER_ID = 0;

int PLAYER_NUMBER = 0;
int PLAYER_CNT = 0;
char (*PLAYER_NAMES)[16] = NULL;

int SIZEX = 0, SIZEY = 0;

int GAME_STATE = 0;

int ROUND = 0;

struct Grid *GRID = NULL;

struct Player_Info *PLAYER_INFO = NULL;