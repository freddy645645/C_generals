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
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <termios.h>

int main() {
    char c[] = "\u2588";
    printf("%s\n", c);
}