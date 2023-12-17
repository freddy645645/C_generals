#ifndef GAME_INCLUDED
#define GAME_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "data_format.h"

void Register_Room(struct Header_Base**,size_t*,struct Header_Room_Register*);
void Join_Room(struct Header_Base**,size_t*,struct Header_Room_Join*);
void Start_Game(struct Header_Base**,size_t*, struct Header_Start_Game*);
void Room_Info(struct Header_Base**,size_t*, struct Header_Room_Info* );
void Add_Action(struct Header_Base**,size_t*, struct Header_Action*);
void Quit_Game(struct Header_Base**,size_t*, struct Header_Quit*);
void Map_Info(struct Header_Base**,size_t*,struct Header_Map_Info*);
void Get_Player_Info(struct Header_Base**,size_t*,struct Header_Player_Info*);
void GameHandler();
void onError(struct Header_Error_Res** ,size_t* ,int , int ,const char * );


#ifdef __cplusplus
} // closing brace for extern "C"
#endif

#endif