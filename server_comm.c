#include "server_comm.h"
void htonl_arr(int * data,int len){
    for (int i=0;i<len;++i)  data[i]=htonl(data[i]);
    //printf("DONE\n");
}
void ntohl_arr(int * data,int len){
    for (int i=0;i<len;++i)  data[i]=ntohl(data[i]);
}

int sendData(int fd, struct Header_Base * header,size_t len){
    switch(header->code){
        case RES_REGISTER_SUCC  : case RES_JOIN_SUCC : case RES_START_GAME_SUCC : case RES_ROOM_INFO_SUCC :{
            struct Header_Room_Info_Res* data=calloc(1,len);
            memcpy(data,header,len);
            htonl_arr((int *)(data),8);

            //for (int i=0;i<len/4;++i) printf("%x\n",((int *)(data))[i]);
            
            int re= writen(fd,data,len);
            free(data);
            return re;

        break;
        }
        case RES_REGISTER_FAIL: case RES_JOIN_FAIL: case RES_START_GAME_FAIL: case RES_ROOM_INFO_FAIL:case RES_RECV_FAIL:{
            struct Header_Error_Res* data=calloc(1,len);
            memcpy(data,header,len);
            htonl_arr((int *)data,2);
            int re= writen(fd,data,len);
            free(data);
            return re;
        break;
        }
        default:
            fprintf(stderr,"Code Error %d\n",header->code);
            return -1;

    }

}

int recvData(int fd,struct Header_Base ** header, size_t* len){
    struct Header_Base base;
    int re=readn(fd,&base,HEADER_SIZE);
    printf("recvData: %d\n",re);
    if(re!=HEADER_SIZE) return -1;
    for (int i=0;i<re/4;++i) printf("%8x ",((int *)(&base))[i]);
    puts("");

    switch(ntohl(base.code)){
        case CMD_REGISTER  :{
            struct Header_Base* ptr=malloc(HEADER_SIZE);

            memcpy(ptr,&base,HEADER_SIZE);
            ntohl_arr((int *)ptr,6);
            //for (int i=0;i<re/4;++i) printf("%x\n",((int *)(ptr))[i]);
            //printf("%p %p\n",header,*header);

            *header=ptr;
            *len=re;
            return re;
        break;
        }
        case CMD_JOIN  : case CMD_START_GAME: case CMD_ROOM_INFO:{
            struct Header_Base* ptr=malloc(HEADER_SIZE);

            memcpy(ptr,&base,HEADER_SIZE);
            ntohl_arr((int *)ptr,4);

            *header=ptr;
            *len=re;
            return re;

        break;
        }

        default:{
            fprintf(stderr,"Code Error %d\n",ntohl(base.code));
            return -2;
        }
    }
}