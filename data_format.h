#ifndef DATA_FORMAT_INCLUDED
#define DATA_FORMAT_INCLUDED
#include "unp.h"
#include <assert.h>


#define CMD_REGISTER 0x1
#define CMD_JOIN     0x2
#define CMD_START_GAME 0x3
#define CMD_ROOM_INFO  0x4

/*
#define CMD_ACTION     0x5
#define CMD_MAP        0x6
#define CMD_QUIT       0x7
*/

#define RES_REGISTER_SUCC      0xA01
#define RES_REGISTER_FAIL      0xF01
#define RES_JOIN_SUCC          0xA02
#define RES_JOIN_FAIL          0xF02
#define RES_START_GAME_SUCC    0xA03
#define RES_START_GAME_FAIL    0xF03
#define RES_ROOM_INFO_SUCC     0xA04
#define RES_ROOM_INFO_FAIL     0xF04


#define RES_RECV_FAIL          0xF05
#define HEADER_SIZE 64

struct Header_Base{
    int32_t code;
    int32_t session;
    int32_t others[14];
}__attribute__ ((aligned (4)));



struct Header_Error_Res{
    int32_t code;
    int32_t session;
    char errmsg[(HEADER_SIZE-2*sizeof(int32_t))];
}__attribute__ ((aligned (4)));


struct Header_Room_Register{
    int code;
    int session; 
    int room_id;
    int player_number;
    int sizeX;
    int sizeY;
    char name[16];
    char passwd[16];
    int unused[2];
}__attribute__ ((aligned (4)));


struct Header_Room_Join{
    int code;
    int session; 
    int room_id;
    int player_id;
    char name[16];
    char passwd[16];
    int unused[4];
}__attribute__ ((aligned (4)));   

struct Header_Room_Info{
    int code;
    int session;
    int room_id;
    int player_id;
    int unused[12];
}__attribute__ ((aligned (4)));

struct Header_Room_Info_Res{
    int code;
    int session;
    int room_id;
    int player_id;
    
    int player_number;
    int sizeX;
    int sizeY;

    int player_cnt;
    char player_names[2][16];
}__attribute__ ((aligned (4)));

struct Header_Start_Game{
    int32_t code;
    int32_t session;
    int room_id;
    int player_id;
    int unused[12];
}__attribute__ ((aligned (4)));

/*
struct Data_Room_Info_Res{
    struct Header_Room_Info_Res;
    char player_names[player_cnt-2];

}
*/
struct Grid{

};

void Header_size_check();







#endif
