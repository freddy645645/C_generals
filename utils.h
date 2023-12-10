

#include "unp.h"

#define isEmptyList(x) (x.prev==x.next)
#define notEmptyList(x) (x.prev==x.next)

#define InitList(x) (x.prev=x.next=&x)
#define push_backList(L,x)  (x->prev=L.prev, x->next=&L, L.prev->next=x, L.prev=x)
#define push_frontList(L,x) (x->prev=&L, x->next=L.next, L.next->prev=x, L.next=x)
#define pop_backList(L,x)  (assert(notEmptyList(L)), x=L.prev, L.prev->prev->next=&L, L.prev=L.prev->prev)
#define pop_frontList(L,x) (assert(notEmptyList(L)), x=L.next, L.next->next->prev=&L, L.next=L.next->next)

struct Node_Player{
    int session; 
    int room_id;
    int player_id;
    int fd;
    char name[16];
};


struct Queue_Action{
    struct Queue_Action *prev, *next;


};


#define STATE_GAME_NOT_GOING 0xC01
#define STATE_GAME_GOING     0xC02
#define STATE_GAME_ENDED     0xC03
struct List_Room{
    struct List_Room *prev, *next;

    struct Node_Player* players;
    int player_number;
    int player_cnt;
    
    int gameState;
    int round;
    int room_id;

    struct Queue_Action *actions;
    
    int sizeX;
    int sizeY;
    struct Grid * map;
    char passwd[16];
};

