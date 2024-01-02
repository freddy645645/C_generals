#include	"unp.h"
#include    "data_format.h"
#include    "server_comm.h"
#include    "game.h"
#include	<stdlib.h>
#include	<stdio.h>
#include    <stdbool.h>
#include <pthread.h>

#define MAX_PLAYER 100

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

fd_set allset;
int fdCnt;
void corruptedClose(int fd){
    --fdCnt;
    Close(fd);
    FD_CLR(fd,&allset);
}



void commandHandler(int fd,struct Header_Base* _header, size_t len){
    struct Header_Base* res=NULL;
    size_t reslen=0;
    puts("commandHandler start");
    switch (_header->code){
        case CMD_REGISTER :{
            Register_Room(&res,&reslen,(struct Header_Room_Register*)_header);
            break;
        }
        case CMD_JOIN     :{
            Join_Room(&res,&reslen,(struct Header_Room_Join*)_header);
            break;
        }
        case CMD_ROOM_INFO  :{
            Room_Info(&res,&reslen,(struct Header_Room_Info*)_header);
            break;
        }
        case CMD_START_GAME :{
            Start_Game(&res,&reslen,(struct Header_Start_Game*)_header);
            break;
        }case CMD_ACTION:{
            Add_Action(&res,&reslen,(struct Header_Action*)_header);
            break;
        }case CMD_QUIT:{
            Quit_Game(&res,&reslen,(struct Header_Quit*)_header);
            break;
        }case CMD_MAP_INFO:{
            Map_Info(&res,&reslen,(struct Header_Map_Info*)_header);
            break;
        }case CMD_PLAYER_INFO:{
            Get_Player_Info(&res,&reslen,(struct Header_Player_Info*)_header);
        }

        default:{
            //CODE ERROR
            printf("Code Error: %d\n",_header->code);
            onError((struct Header_Error_Res**)&res,&reslen,0,RES_RECV_FAIL,"Code Error");
        }
    }
    int re=sendData(fd,res,reslen);
    free(res);
    if(re<=0) corruptedClose(fd);

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
    fdCnt = 0;

    FD_ZERO(&allset);
	FD_SET(listenfd, &allset);

    srand(time(NULL));
    printf("thread start\n");
    pthread_t gameHandle; 
    int pthread_res=pthread_create(&gameHandle, NULL, GameHandler, NULL);
    if(pthread_res!=0){
        printf("GameHandler start fail");
    }else{
        printf("GameHandler Start");
    }
	for(; ; ) {
		rset = allset;		
		nready = Select(maxfd+1, &rset, NULL, NULL, NULL);

		if (FD_ISSET(listenfd, &rset) ) {	
			clilen = sizeof(cliaddr);
            if(fdCnt<MAX_PLAYER){
                connfd = Accept(listenfd, (SA *) &cliaddr, &clilen);
			    if(connfd>0){
                    FD_SET(connfd,&allset);
                    ++fdCnt;
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
                    onError((struct Header_Error_Res**)&header, &len,0, RES_RECV_FAIL,"Recv Data Error");
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

