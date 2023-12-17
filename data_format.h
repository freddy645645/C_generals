#ifndef DATA_FORMAT_INCLUDED
#define DATA_FORMAT_INCLUDED
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define CMD_REGISTER 0x1
#define CMD_JOIN     0x2
#define CMD_START_GAME 0x3
#define CMD_ROOM_INFO  0x4
#define CMD_ACTION     0x5
#define CMD_MAP_INFO   0x6
#define CMD_QUIT       0x7
#define CMD_PLAYER_INFO 0x8


#define RES_REGISTER_SUCC      0xA01
#define RES_REGISTER_FAIL      0xF01
#define RES_JOIN_SUCC          0xA02
#define RES_JOIN_FAIL          0xF02
#define RES_START_GAME_SUCC    0xA03
#define RES_START_GAME_FAIL    0xF03
#define RES_ROOM_INFO_SUCC     0xA04
#define RES_ROOM_INFO_FAIL     0xF04
#define RES_ACTION_SUCC        0xA05
#define RES_ACTION_FAIL        0xF05
#define RES_MAP_INFO_SUCC      0xA06
#define RES_MAP_INFO_FAIL      0xF06
#define RES_QUIT_SUCC          0xA07
#define RES_QUIT_FAIL          0xF07
#define RES_PLAYER_INFO_SUCC   0xA08
#define RES_PLAYER_INFO_FAIL   0xF08


#define RES_RECV_FAIL          0xF09
#define HEADER_SIZE 64


#define GAME_STATE_WAIT        0x00
#define GAME_STATE_INGAME      0x01
#define GAME_STATE_END         0x02

#define PLAYER_STATE_ALIVE    0x0
#define PLAYER_STATE_QUITED   0x1
#define PLAYER_STATE_DEAD     0x2


#define GAME_MAP_FOG        -2
#define GAME_MAP_MOUNTAIN   -3
#define GAME_MAP_CASTLE     -4

#define GAME_MAP_OWN_NEUTRAL   -1

#define HEADER_SIZE 64

struct Header_Base{
    int32_t code;
    int32_t session;
    int32_t others[14];
}__attribute__ ((aligned (4)));



// on Error: 0xFxx
// ok: 0xAxx
struct Header_Error_Res{
    int32_t code;
    int32_t session;
    int32_t unused[6];
    char errmsg[32];
}__attribute__ ((aligned (4)));



struct Header_Room_Register{
    int32_t code;
    int32_t session; 
    int32_t room_id;
    int32_t player_number;
    int32_t sizeX;
    int32_t sizeY;
    int32_t unused[2];

    char name[16];
    char passwd[16];
}__attribute__ ((aligned (4)));
 
struct Header_Room_Join{
    int32_t code;
    int32_t session; 
    int32_t room_id;

    int32_t unused[5];

    char name[16];
    char passwd[16];
}__attribute__ ((aligned (4)));   

struct Header_Room_Info{
    int32_t code;
    int32_t session;
    int32_t room_id;
    int32_t player_id;
    int32_t unused[12];
}__attribute__ ((aligned (4)));

struct Header_Start_Game{
    int32_t code;
    int32_t session;
    int32_t room_id;
    int32_t player_id;
    int32_t unused[12];
}__attribute__ ((aligned (4)));

struct Header_Room_Info_Res{
    int32_t code;
    int32_t session;
    int32_t room_id;
    int32_t player_id;
    
    int32_t player_number;
    int32_t sizeX;
    int32_t sizeY;

    int32_t game_state;
    char player_names[2][16];
}__attribute__ ((aligned (4)));
/*
struct Data_Game_info_Res{
    Header_Room_Info_Res;
    char names[player_numbers][16];
}
*/

struct Header_Quit{
    int32_t code;
    int32_t session;
    int32_t room_id;
    int32_t player_id;
    int32_t unused[12];
}__attribute__ ((aligned (4)));
//struct Header_Quit_Res == Header_Error_Res
// code and message  different



struct Action{
    int32_t x;
    int32_t y;
    int32_t all_or_half;
    int32_t way;
}__attribute__ ((aligned (4)));
struct Header_Action{
    int32_t code;
    int32_t session;
    int32_t room_id;
    int32_t player_id;
    
    int32_t num_Action;
    int32_t unused[3];
    struct Action action[2];
}__attribute__ ((aligned (4)));


/*
struct Data_Action{
    Header_Action Header;
    Action action[num_Action-2];
};
*/
struct Header_Action_Res{
    int32_t code;
    int32_t session;
    int32_t room_id;
    int32_t player_id;
    int32_t num_Action;
    int32_t unused[11];
}__attribute__ ((aligned (4)));





struct Header_Map_Info{
    int32_t code;
    int32_t session;
    int32_t room_id;
    int32_t player_id;
    int32_t game_state;
    int32_t unused[11];
}__attribute__ ((aligned (4)));
struct Grid{
    int32_t type;
    int32_t owner;
    int32_t soldier_num;
    int32_t unused;
}__attribute__ ((aligned (4)));
struct Header_Map_Info_Res{
    int32_t code;
    int32_t session;
    int32_t room_id;
    int32_t player_id;
    
    int32_t sizeX;
    int32_t sizeY;
    int32_t round;
    int32_t game_state;
    struct Grid grid[2];
}__attribute__ ((aligned (4)));
/*
struct Data_Map{
    Header_map  header;
    Grid [sizeX*sizeY-TBD];
    
}
*/



struct Header_Player_Info{
    int32_t code;
    int32_t session;
    int32_t room_id;
    int32_t player_id;
    int32_t unused[12];
}__attribute__ ((aligned (4)));
struct Player_Info{
    int32_t grid_num;
    int32_t soldier_num;
    int32_t player_state;
    int32_t unused;
}__attribute__ ((aligned (4)));

struct Header_Player_Info_Res{
    int32_t code;
    int32_t session;
    int32_t room_id;
    int32_t player_id;
    
    
    int32_t game_state;
    int32_t unused[3];
    struct Player_Info player[2];
}__attribute__ ((aligned (4)));

void Header_size_check();







#endif
