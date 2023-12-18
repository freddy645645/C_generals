#ifndef SERVER_COMM_INCLUDED
#define SERVER_COMM_INCLUDED
#include "data_format.h"
#include	"unp.h"
#define getLen(x) (((uint32_t)(x))>>16)
#define clrLen(x) (((x))&=((1<<16)-1))
#define setLen(x,len) ((x)|=(len<<16))
#define getCode(x)    ((x)&((1<<16)-1))
void htonl_arr(int * ,int);

void ntohl_arr(int * ,int);

int sendData(int , struct Header_Base* ,size_t);


int  recvData(int , struct Header_Base** ,size_t*);





#endif