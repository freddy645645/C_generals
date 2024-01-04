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

#include <stdio.h>

int main() {
  int column_width = 20;

  printf("%-*s | %-*s | %-*s\n", column_width, "Number", column_width,
         "Description", column_width, "Value");
  printf("%-*c | %-*c | %-*c\n", column_width, '-', column_width, '-',
         column_width, '-');

  printf("%-*d | %-*s | %*.*d\n", column_width, 123, column_width, "First",
         column_width, 2, 4567);
  printf("%-*d | %-*s | %*.*d\n", column_width, 456, column_width, "Second",
         column_width, 4, 789);
  printf("%-*d | %-*s | %*.*d\n", column_width, 7890, column_width, "Third",
         column_width, 1, 12345);

  return 0;
}