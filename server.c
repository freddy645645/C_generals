#include	"unp.h"
#include    "data_format.h"
#include    "server_comm.h"
#include    "utils.h"
#include	<stdlib.h>
#include	<stdio.h>
#include    <stdbool.h>
#define MAX_PLAYER 100
int getSessin(){
    static int state=0;
    if(!state) state=rand();
    state=(state*20240101)+54321;
    return state;
}
/*List_Room{
    List_player*;
    int room_id;
    Queue_Cmd;

};*/
/*void test(int fd){
    srand(time(NULL));
    struct Header_Error_Res h1;
    memset(&h1,0,HEADER_SIZE);
    h1.code=RES_JOIN_FAIL;
    for(int i=0;i<8;++i)
        printf("%x\n",((int*)(&h1))[i]);
    h1.session=getSessin();
    sprintf(h1.errmsg,"This is Error Testing");
    printf("Session h1: %d\n",h1.session);
    printf("%s\n",h1.errmsg);
    int re=sendData(fd,(struct Header_Base*)(&h1),sizeof(h1));
    printf("%d\n",re);

    int num=4;
    struct Header_Room_Info_Res *h2=calloc(1,HEADER_SIZE+(num-2)*16);
    h2->code=RES_JOIN_SUCC;
    
    h2->session=getSessin();
    h2->room_id=5;
    h2->player_id=0;
    h2->sizeX=h2->sizeY=10;
    //h2->player_cnt=num;
    printf("Session h2: %x\n",h2->session);
    sprintf(h2->player_names[0],"123456789");
    sprintf(h2->player_names[1],"987654321");
    sprintf(h2->player_names[2],"456789123");
    sprintf(h2->player_names[3],"196453421");
    re=sendData(fd,(struct Header_Base*)(h2),HEADER_SIZE+(num-2)*16);
    printf("%d\n",re);


    printf("test3\n");
    struct Header_Room_Register* h3=NULL;
    size_t len=0;
    //printf("%p\n",h3);

    re=recvData(fd,(struct Header_Base**)(&h3),&len);
    //printf("%d\n",re);
    // printf("%p %p\n",&h3,h3);
    
    for (int i=0;i<6;++i) printf("%x\n",((int *)(h3))[i]);
    printf("%s\n%s\n",h3->name,h3->passwd);

    printf("test4\n");
    struct Header_Room_Join* h4=NULL;
    len=0;
    //printf("%p\n",h3);

    re=recvData(fd,(struct Header_Base**)(&h4),&len);
    //printf("%d\n",re);
    // printf("%p %p\n",&h3,h3);
    
    for (int i=0;i<6;++i) printf("%x\n",((int *)(h4))[i]);
    printf("%s\n%s\n",h4->name,h4->passwd);


    close(fd);



}
*/
struct List_Room listRoom;
struct Node_Player* players[MAX_PLAYER+10];
fd_set allset;
int playerCnt;
void corruptedClose(int fd){
    if (players[fd]){
        players[fd]->fd=-1;
        players[fd]=NULL;
    }
    Close(fd);
    FD_CLR(fd,&allset);
}
struct List_Room* getRoom(int room_id){
    struct List_Room* ptr=listRoom.next;
    while(ptr!=(&listRoom) && ptr->room_id!=room_id) ptr=ptr->next;
    return ptr->room_id==room_id ? ptr: NULL;
}
int onError(int fd,int session, int errcode,const char * errmsg){
    struct Header_Error_Res err;
    err.code=errcode;
    err.session=session;
    snprintf(err.errmsg,sizeof(err.errmsg),"%s",errmsg);
    int re=sendData(fd,(struct Header_Base*)&err,sizeof(err));
    return re;
}
int sendGameInfo(struct List_Room* room,int player_id,int code){
    struct Node_Player* player=&(room->players[player_id]);
    size_t len=HEADER_SIZE+16*(max(0,room->player_number-2));
    struct Header_Room_Info_Res* header=calloc(1,len);
    header->code=code;
    header->session=player->session;
    header->room_id=player->room_id;
    header->player_id=player->player_id;
    header->player_number=room->player_number;
    header->sizeX=room->sizeX;
    header->sizeY=room->sizeY;
    //header->player_cnt=room->player_cnt;
    for (int i=0;i<room->player_number;++i)
        memcpy(header->player_names[i],(room->players[i]).name,16);
    int re=sendData(player->fd,(struct Header_Base*)header,len);
    free(header);

    return re;
        
}
bool verifySession(struct Header_Room_Info* header){
    struct List_Room* room=getRoom(header->room_id);
    if(room==NULL) return false;
    if (header->player_id>=room->player_number||header->session!=room->players[header->player_id].session)
        return false;
    return true;
}
void commandHandler(int fd,struct Header_Base* _header, size_t len){

    switch (_header->code){
        case CMD_REGISTER :{
            struct Header_Room_Register* header=(struct Header_Room_Register*) _header;
            struct List_Room* room=getRoom(header->room_id);
            if(room==NULL){
                room=calloc(1,sizeof(struct List_Room));
                
                struct List_Room *tmpP;
                tmpP=listRoom.prev;
                
                push_backList(listRoom,room);

                assert(room->next==&listRoom);assert(room->prev==tmpP);
                assert(listRoom.prev==room);  assert(tmpP->next==room);
                
                room->player_number=header->player_number;
                room->players=calloc(room->player_number,sizeof(struct Node_Player));
                room->player_cnt=0;
                
                room->gameState=STATE_GAME_NOT_GOING;
                room->round=0;
                room->room_id=header->room_id;

                room->sizeX=header->sizeX;
                room->sizeY=header->sizeY;
                room->map=calloc(room->sizeX*room->sizeY,sizeof(struct Grid));

                memcpy(room->passwd,header->passwd,16);

                struct Node_Player* player=&(room->players[room->player_cnt]);
                player->session=getSessin();
                player->room_id=room->room_id;
                player->player_id=room->player_cnt;room->player_cnt+=1;
                player->fd=fd;
                memcpy(player->name,header->name,16);
                
                printf("Room %d '%s' Added %d\n",header->room_id,player->name,room->player_cnt);

                int re=sendGameInfo(room,player->player_id,RES_JOIN_SUCC);
                if(re<=0) corruptedClose(fd);

            }else{
                int re=onError(fd, header->session, RES_REGISTER_FAIL,"Room Already Exist");
                if(re<=0) corruptedClose(fd);
            }

            break;
        }
        case CMD_JOIN     :{
            struct Header_Room_Join* header=(struct Header_Room_Join*) _header;
            struct List_Room* room=getRoom(header->room_id);
            if(room==NULL){
                int re=onError(fd, header->session, RES_JOIN_FAIL,"Room Doesn't Exist");
                if(re<=0) corruptedClose(fd);
            }else if(room->player_cnt==room->player_number){
                int re=onError(fd, header->session, RES_JOIN_FAIL,"Room Is Full");
                if(re<=0) corruptedClose(fd);
            }else if (strcmp(room->passwd,header->passwd)){
                int re=onError(fd, header->session, RES_JOIN_FAIL,"Password Is Wrong");
                if(re<=0) corruptedClose(fd);

            }else{
                struct Node_Player* player=&(room->players[room->player_cnt]);
                player->session=getSessin();
                player->room_id=room->room_id;
                player->player_id=room->player_cnt++;
                player->fd=fd;
                memcpy(player->name,header->name,16);

                printf("Room %d '%s' Added %d\n",header->room_id,player->name,room->player_cnt);
                

                int re=sendGameInfo(room,player->player_id,RES_JOIN_SUCC);
                if(re<=0) corruptedClose(fd);

                
            }   
            break;
        }
        case CMD_ROOM_INFO  :{
            struct Header_Room_Info* header=(struct Header_Room_Info*) _header;
            struct List_Room* room=getRoom(header->room_id);
            if (verifySession(header)==false){
                int re=onError(fd, header->session, RES_ROOM_INFO_FAIL,"Player Verify Error");
                if(re<=0) corruptedClose(fd);
            }else{
                struct Node_Player* player=&(room->players[header->player_id]);

                printf("Room %d '%s' Info Send\n",header->room_id,player->name);
                

                int re=sendGameInfo(room,player->player_id,RES_JOIN_SUCC);
                if(re<=0) corruptedClose(fd);

                
            } 
            break;
        }
        case CMD_START_GAME :{
            struct Header_Room_Info* header=(struct Header_Room_Info*) _header;
            struct List_Room* room=getRoom(header->room_id);
            if (verifySession(header)==false){
                int re=onError(fd, header->session, RES_ROOM_INFO_FAIL,"Player Verify Error");
                if(re<=0) corruptedClose(fd);
            }else if(room->gameState!=STATE_GAME_NOT_GOING){
                int re=onError(fd, header->session, RES_START_GAME_FAIL,"Game Already Start");
                if(re<=0) corruptedClose(fd);
            }
            else{
                struct Node_Player* player=&(room->players[header->player_id]);

                printf("Room %d '%s' Game Send\n",header->room_id,player->name);
                
                for(int i=0;i<room->player_cnt;++i){
                    int re=sendGameInfo(room,i,RES_START_GAME_SUCC);
                    if(re<=0) corruptedClose(fd);
                }
                
                
            } 
            break;
        }

        default:{
            //CODE ERROR


        }
    }




}




int main(int argc, char **argv)
{
    int			listenfd, connfd;
    socklen_t		clilen;
    struct sockaddr_in	cliaddr, servaddr;
    fd_set	            rset;
    int					nready;   
    int                 maxfd;

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);
    int flags = fcntl(listenfd, F_GETFL, 0);
    fcntl(listenfd, F_SETFL, flags | O_NONBLOCK);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(SERV_PORT+3);
    printf("%d\n",SERV_PORT+3);
    Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

    Listen(listenfd, LISTENQ);

    maxfd = listenfd;	
    playerCnt = 0;
    memset(players,0,sizeof(players));
    InitList(listRoom);

    FD_ZERO(&allset);
	FD_SET(listenfd, &allset);



	for(; ; ) {
		rset = allset;		
		nready = Select(maxfd+1, &rset, NULL, NULL, NULL);

		if (FD_ISSET(listenfd, &rset) ) {	
			clilen = sizeof(cliaddr);
            if(playerCnt<MAX_PLAYER){
                connfd = Accept(listenfd, (SA *) &cliaddr, &clilen);
			    if(connfd>0){
                    FD_SET(connfd,&allset);
                    ++playerCnt;
                    maxfd=max(maxfd,connfd);

                    printf("CONNFD:%d\n",connfd);
                    
                }
            }

			if (--nready <= 0)
				continue;				
		}

		for (int fd = listenfd+1; fd <= maxfd; fd++) {
            if(FD_ISSET(fd,&rset)){
                struct Header_Base* header=NULL;
                size_t len;
                int re=recvData(fd,&header,&len);
                printf("RECV:%d\n",re);
                if(re<=0){
                    re=onError(fd, 0, RES_RECV_FAIL,"Recv Data Error");
                    corruptedClose(fd);
                }else{
                    commandHandler(fd,header,len);
                }

                free(header);
                
                if (--nready <= 0)
                break;			
            }
		}
	}

}

