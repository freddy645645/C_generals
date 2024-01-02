#ifndef CLIACTION_INCLUDED
#define CLIACTION_INCLUDED

#include "globals.h"
#include "data_format.h"

#include <netinet/in.h>
#include <net/if.h>
#include <netdb.h>
#include <unistd.h>
#include <ifaddrs.h>

#include <arpa/inet.h>

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <memory.h>
#include <stdio.h>
#include <time.h>

#define BUF_SIZE 1024

#define getLen(x)       (((uint32_t)(x))>>16)
#define clrLen(x)       (((x))&=((1<<16)-1))
#define setLen(x,len)   ((x)|=(len<<16))
#define getCode(x)      ((x)&((1<<16)-1))


void htonl_arr(int *data, int len);
void ntohl_arr(int *data, int len);

void status_mes(char *mes);
void error_mes(char *mes);
void error_check(struct Header_Error_Res *header);

void send_data(int fd, struct Header_Base *header, size_t len);
void recv_data(int fd, struct Header_Base *header, size_t *len);

void connect_server(char *ip, char *port);

void room_register(int roomID, int playerNum, int szX, int szY,
    char *name, char *passwd);

struct Header_Room_Info_Res room_info();

#endif