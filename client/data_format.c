#include "data_format.h"

void Header_size_check(){
    static_assert ((sizeof(struct Header_Base)) == HEADER_SIZE, "Header_Base size Error");
    static_assert ((sizeof(struct Header_Error_Res)) == HEADER_SIZE, "Header_Error_Res size Error");
    static_assert ((sizeof(struct Header_Room_Register)) == HEADER_SIZE, "Header_Room_Register size Error");
    static_assert ((sizeof(struct Header_Room_Join)) == HEADER_SIZE, "Header_Room_Join size Error");
    static_assert ((sizeof(struct Header_Room_Info)) == HEADER_SIZE, "Header_Room_Info size Error");
    static_assert ((sizeof(struct Header_Room_Info_Res)) == HEADER_SIZE, "Header_Room_Info_Res size Error");
    static_assert ((sizeof(struct Header_Start_Game)) == HEADER_SIZE, "Header_Start_Game size Error");
    static_assert ((sizeof(struct Header_Quit)) == HEADER_SIZE, "Header_Quit size Error");
    static_assert ((sizeof(struct Header_Action)) == HEADER_SIZE, "Header_Action size Error");
    static_assert ((sizeof(struct Header_Action_Res)) == HEADER_SIZE, "Header_Action_Res size Error");
    static_assert ((sizeof(struct Header_Map_Info)) == HEADER_SIZE, "Header_Map_Info size Error");
    static_assert ((sizeof(struct Header_Map_Info_Res)) == HEADER_SIZE, "Header_Map_Info_Res size Error");
    static_assert ((sizeof(struct Header_Player_Info)) == HEADER_SIZE, "Header_Player_Info size Error");
    static_assert ((sizeof(struct Header_Player_Info_Res)) == HEADER_SIZE, "Header_Player_Info_Res size Error");
}