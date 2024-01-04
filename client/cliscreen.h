#ifndef CLISCREEN_INCLUDED
#define CLISCREEN_INCLUDED

#include "data_format.h"
#include "cliaction.h"
#include <sys/ioctl.h>
#include <sys/select.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>

void move_at(int h, int w);

void get_screen_size(int *h, int *w);

void clear_screen();
void clear_line(int h);
void clear_after(int h, int w);

void cursor_visable(int x);

void disable_echo();
void enable_echo();

void disable_cbreak();
void enable_cbreak();

void print_at(const char *buf, int h, int w);
int print_middle(const char *buf, int h);
void print_at_slow(const char *buf, int h, int w);
void print_loading(const char *buf, int h, int w);

void read_at(char *buf, int h, int w);
int read_at_wait(char *buf, int h, int w, int s, int us);
void get_at(char *c, int h, int w);

int read_server_check(int s, int us);

#endif
