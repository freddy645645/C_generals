#include "cliscreen.h"

void move_at(int h, int w) {
    // move cursor to h, w
    printf("\x1B[%d;%dH", h, w);
}

void clear_screen() {
    printf("\x1B[2J");
}

void clear_line(int h) {
    move_at(h, 0);
    printf("\x1B[2K");
}

void clear_after(int h, int w) {
    move_at(h, w);
    printf("\x1B[0K");
}

void cursor_visable(int x) {
    if(x)
        printf("\x1B[?25h");
    else
        printf("\x1B[?25l");
    return;
}

void disable_echo() {
    // disable input echo on terminal
    struct termios term;
    tcgetattr(fileno(stdin), &term);

    term.c_lflag &= ~ECHO;
    tcsetattr(fileno(stdin), 0, &term);
}

void enable_echo() {
    struct termios term;
    tcgetattr(fileno(stdin), &term);

    term.c_lflag |= ECHO;
    tcsetattr(fileno(stdin), 0, &term);
}

void disable_cbreak() {
    // avoid pressing enter to read
    struct termios term;
    tcgetattr(fileno(stdin), &term);

    term.c_lflag &= ~ICANON;
    tcsetattr(fileno(stdin), 0, &term);
}

void enable_cbreak() {
    struct termios term;
    tcgetattr(fileno(stdin), &term);

    term.c_lflag |= ICANON;
    tcsetattr(fileno(stdin), 0, &term);
}

void get_screen_size(int *h, int *w) {
    struct winsize win;
    ioctl(fileno(stdout), TIOCGWINSZ, &win);
    *h = win.ws_row;
    *w = win.ws_col;
    return;
}

void print_at(const char *buf, int h, int w) {
    printf("\x1B[%d;%dH%s", h, w, buf);
}

void print_at_slow(const char *buf, int h, int w) {
    for(int i = 0; i < strlen(buf); ++i) {
        printf("\x1B[%d;%dH", h, w);
        for(int j = 0; j <= i; ++j)
            printf("%c", buf[j]);
        fflush(stdout);
        usleep(300000);
    }
}

void print_loading(const char *buf, int h, int w) {
    char block[] = "\u2588";
    for(int i = 1; i <= 50; ++i) {
        clear_after(h, w);

        move_at(h, w);
        printf("%s: [", buf);
        for(int j = 1; j <= i; ++j)
            printf("%s", block); 
        for(int j = i + 1; j <= 50; ++j)
            printf("-");
        printf("]\n");
        usleep(50000);
    }
}

void read_at(char *buf, int h, int w) {
    memset(buf, 0, sizeof(buf));
    enable_echo();
    move_at(h, w);
    scanf("%s", buf);
    disable_echo();
}

void get_at(char *c, int h, int w) {
    enable_echo();
    move_at(h, w);
    scanf("%c", c);
    disable_echo();
}