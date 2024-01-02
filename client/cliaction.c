#include "cliaction.h"

void htonl_arr(int * data,int len) {
    for(int i = 0; i < len; ++i)
        data[i]=htonl(data[i]);
}

void ntohl_arr(int * data,int len) {
    for(int i = 0; i < len; ++i)
        data[i]=ntohl(data[i]);
}

void status_mes(char *mes) {
    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    FILE *fptr = fopen("status.log", "a");
    char curTime[BUF_SIZE];
    strcpy(curTime, asctime(timeinfo));
    curTime[strlen(curTime) - 1] = 0;
    fprintf(fptr, "%s: %s\n", curTime, mes);
    fclose(fptr);
}

void error_mes(char *mes) {
    char err[BUF_SIZE];
    sprintf(err, "[Error] %s", mes) ;
    status_mes(err);
    exit(1);
}

void error_check(struct Header_Error_Res *header) {
    int errorCode = 0xF00;
    if((header->code & errorCode) == errorCode) {
        error_mes(header->errmsg);
    }
}


void send_data(int fd, struct Header_Base *header, size_t len) {
    struct Header_Base *data = calloc(1, len);
    memcpy(data, header, len);
    setLen(data->code, len);
    
    switch(header->code){
        case CMD_REGISTER: {
            htonl_arr((int *)(data), 8);
            break;
        }
        default: {
            free(data);
            error_mes("send_data error (not found header code)");
        }
    }

    status_mes("sending data");
    size_t re = write(fd ,data, len);
    free(data);
    
    if(re != len) {
        error_mes("send_data error (send length error)");
    }

    return;
}

void recv_data(int fd, struct Header_Base *header, size_t *len){
    struct Header_Base base;

    status_mes("reading header");
    int re = read(fd, &base, HEADER_SIZE);
    if(re != HEADER_SIZE) {
        error_mes("recv_data HEADER_SIZE error");
    }

    int dataLen = getLen(ntohl(base.code));

    struct Header_Base *data = calloc(1, dataLen);
    memcpy(data, &base, HEADER_SIZE);

    if(dataLen != HEADER_SIZE){
        status_mes("reading extra data");
        re = read(fd, ((void *)(&data))+HEADER_SIZE, dataLen-HEADER_SIZE);
        if(re+HEADER_SIZE != dataLen) {
            free(data);
            error_mes("recv_data data length error");
        }
    }

    int code = getCode(ntohl(base.code));
    switch(code) {
        case RES_REGISTER_SUCC: case RES_REGISTER_FAIL: {
            // return Header_Room_Info_Res
            htonl_arr((int *)(data), 8);
            break;
        }
        default:{
            free(data);
            error_mes("recv_data code error");
        }
    }
    clrLen(data->code);
    memcpy(header, data, dataLen);
    *len = dataLen;
    free(data);

    error_check((struct Header_Error_Res*)(header));
    return;
}

void connect_server(char *ip, char *port) {
    struct sockaddr_in servaddr;
    SOCKFD = socket(AF_INET, SOCK_STREAM, 0);
    memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(port));
	inet_pton(AF_INET, ip, &servaddr.sin_addr);
    status_mes("connecting to server");
	int n = connect(SOCKFD, (struct sockaddr *) &servaddr, sizeof(servaddr));
    if(n == -1) {
        error_mes("connect error");
    }
}


void room_register(int roomID, int playerNum, int szX, int szY,
    char *name, char *passwd) {
    struct Header_Room_Register myRegister;
    memset(&myRegister, 0, sizeof(myRegister));

    myRegister.code = CMD_REGISTER;
    myRegister.room_id = roomID;
    myRegister.player_number = playerNum;
    myRegister.sizeX = szX; 
    myRegister.sizeY = szY;
    strcpy(myRegister.name, name);
    strcpy(myRegister.passwd, passwd);

    struct Header_Room_Info_Res res;
    size_t resLen;
    status_mes("registering the room");
    send_data(SOCKFD, (struct Header_Base*)(&myRegister), sizeof(myRegister));
    recv_data(SOCKFD, (struct Header_Base*)(&res), &resLen);

    SESSION = res.session; 
    ROOM_ID = res.room_id;
    PLAYER_ID = res.player_id;

    return;
}


struct Header_Room_Info_Res room_info() {

    if(!SOCKFD) {
        error_mes("not connect to server yet");
    }

    struct Header_Room_Info request;
    struct Header_Room_Info_Res result;
    size_t len;
    memset(&request, 0, sizeof(request));
    memset(&result, 0, sizeof(result));

    request.code = CMD_ROOM_INFO;
    request.session = SESSION;
    request.room_id = ROOM_ID;
    request.player_id = PLAYER_ID;

    status_mes("getting room info");
    send_data(SOCKFD, (struct Header_Base*)(&request), sizeof(request));
    recv_data(SOCKFD, (struct Header_Base*)(&result), &len);
}
