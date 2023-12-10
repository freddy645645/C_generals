#ifndef SERVER_COMM_INCLUDED
#define SERVER_COMM_INCLUDED
#include "data_format.h"
#include	"unp.h"
void htonl_arr(int * ,int);

void ntohl_arr(int * ,int);

int sendData(int , struct Header_Base* ,size_t);


int  recvData(int , struct Header_Base** ,size_t*);





#endif