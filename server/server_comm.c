#include "server_comm.h"
void htonl_arr(int * data,int len){
    for (int i=0;i<len;++i)  data[i]=htonl(data[i]);
    //printf("DONE\n");
}
void ntohl_arr(int * data,int len){
    for (int i=0;i<len;++i)  data[i]=ntohl(data[i]);
}

int sendData(int fd, struct Header_Base * header,size_t len){
    struct Header_Base* data=calloc(1,len);
    memcpy(data,header,len);
    data->code=data->code+(len<<16);
    setLen(data->code,len);
    
    switch(header->code){
        case RES_REGISTER_SUCC:case RES_JOIN_SUCC: case RES_START_GAME_SUCC: case RES_ROOM_INFO_SUCC :{
            htonl_arr((int *)(data),8);
            break;
        }
        case RES_REGISTER_FAIL: case RES_JOIN_FAIL: case RES_START_GAME_FAIL: case RES_ROOM_INFO_FAIL:case RES_RECV_FAIL:case RES_ACTION_FAIL:case RES_MAP_INFO_FAIL:case RES_QUIT_FAIL:case RES_QUIT_SUCC:case RES_PLAYER_INFO_FAIL:{
            htonl_arr((int *)data,8);
            break;
        }
        case RES_ACTION_SUCC:case RES_MAP_INFO_SUCC: case RES_PLAYER_INFO_SUCC:{
            htonl_arr((int *)data,len/4);
            break;
        }
        default:
            free(data);
            fprintf(stderr,"Code Error %d\n",header->code);
            return -1;
    }
    //for (int i=0;i<len/4;++i) printf("%x\n",((int *)(data))[i]);
    int re= writen(fd,data,len);
    free(data);
    return re;
}

int recvData(int fd,struct Header_Base ** header, size_t* len){
    struct Header_Base base;
    int re;

    re=readn(fd,&base,HEADER_SIZE);
    printf("recvData: %d\n",re);
    if(re!=HEADER_SIZE) return -1;
    for (int i=0;i<re/4;++i) printf("%8x ",((int *)(&base))[i]);
    puts("");
    int Len=getLen(ntohl(base.code));
    printf("Len: %d\n",Len);
    struct Header_Base* data=calloc(1,Len);
    memcpy(data,&base,HEADER_SIZE);
    if(Len!=HEADER_SIZE){
        re=readn(fd,((void *)(&base))+HEADER_SIZE,Len-HEADER_SIZE);
        printf("recvDataRest: %d\n",re);
        if(re+HEADER_SIZE!=Len) {
            free(data);
            return -1;
        }
    }
    int code=getCode(ntohl(base.code));
    printf("Code: %d\n",code);
    switch(code){
        case CMD_REGISTER  :case CMD_JOIN  : {
            ntohl_arr((int *)data,8);
            break;
        }
        case CMD_START_GAME: case CMD_ROOM_INFO:case CMD_ACTION:case CMD_MAP_INFO:case CMD_QUIT:case CMD_PLAYER_INFO:{
            ntohl_arr((int *)data,Len/4);
             break;
        }
        default:{
            free(data);
            fprintf(stderr,"Code Error %d\n",ntohl(base.code));
            return -2;
        }
    }
    clrLen(data->code);
    //for (int i=0;i<re/4;++i) printf("%x\n",((int *)(data))[i]);
    //printf("%p %p\n",header,*header);
    *header=data;
    *len=Len;
    return Len;

}