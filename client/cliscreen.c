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

int print_middle(const char *buf, int h) {
    int nowH, nowW;
    get_screen_size(&nowH, &nowW);
    int len = strlen(buf);
    int middle = nowW / 2;
    int start = middle - len / 2;
    if(start <= 0)
        start = 1;
    print_at(buf, h, start);
    fflush(stdout);
    return start + len;
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

int read_at_wait(char *buf, int h, int w, int s, int us) {
    fd_set rfds;
    struct timeval tv;
    int retval;
    FD_ZERO(&rfds);
    FD_SET(0, &rfds);

    tv.tv_sec = s;
    tv.tv_usec = us;

    retval = select(1, &rfds, NULL, NULL, &tv);

    if(retval == -1) {
        error_mes("read_at_wait select error");
    }
    else if(retval) {
        memset(buf, 0, sizeof(buf));
        move_at(h, w);
        scanf("%s", buf);
        return 1;
    }
    else {
        return 0;
    }
}

void get_at(char *c, int h, int w) {
    enable_echo();
    move_at(h, w);
    scanf("%c", c);
    disable_echo();
}

int get_at_wait(char *c, int h, int w, int s, int us) {
    fd_set rfds;
    struct timeval tv;
    int retval;
    FD_ZERO(&rfds);
    FD_SET(0, &rfds);

    tv.tv_sec = s;
    tv.tv_usec = us;

    retval = select(1, &rfds, NULL, NULL, &tv);

    if(retval == -1) {
        error_mes("get_at_wait select error");
    }
    else if(retval) {
        move_at(h, w);
        scanf("%c", c);
        return 1;
    }
    else {
        return 0;
    }
}

int read_server_check(int s, int us) {
    fd_set rfds;
    struct timeval tv;
    int retval;
    FD_ZERO(&rfds);
    FD_SET(SOCKFD, &rfds);

    tv.tv_sec = s;
    tv.tv_usec = us;

    retval = select(SOCKFD + 1, &rfds, NULL, NULL, &tv);

    if(retval == -1) {
        error_mes("read_server_check select error");
    }
    else if(retval) {
        return 1;
    }
    else {
        return 0;
    }
}

void make_color(char **str, int idx) {
    char *ret = calloc(BUF_SIZE, 1);
    sprintf(ret, "\x1B[%dm%s\x1B[0m", 41+idx, *str);
    *str = ret;
}

void make_text_color(char **str) {
    char *ret = calloc(BUF_SIZE, 1);
    sprintf(ret, "\x1B[%dm%s\x1B[0m", 36, *str);
    *str = ret;
}


void map_type_soldiers(char **str, int row, int col) {
    char *ret = calloc(4, 1);
    char c;
    if(GRID[row * SIZEY + col].type == GAME_MAP_FOG) {
        c = 'X';
    }
    if(GRID[row * SIZEY + col].type == GAME_MAP_MOUNTAIN) {
        c = '^';
    }
    if(GRID[row * SIZEY + col].type == GAME_MAP_CASTLE) {
        c = '+';
    }
    if(GRID[row * SIZEY + col].type == GAME_MAP_SPACE) {
        c = ' ';
    }
    if(GRID[row * SIZEY + col].type == GAME_MAP_HOME) {
        c = '*';
    }
    ret[0] = c;
    int x = GRID[row * SIZEY + col] .soldier_num;
    ret[3] = x % 10 + '0', x /= 10;
    ret[2] = x % 10 + '0', x /= 10;
    ret[1] = x % 10 + '0', x /= 10;
    if(ret[1] == '0') {
        ret[1] = ' ';
        if(ret[2] == '0') {
            ret[2] = ' ';
            if(ret[3] == '0')
                ret[3] = ' ';
        }
    }
    *str = ret;
}

int map_owner(int row, int col) {
    return GRID[row * SIZEY + col].owner;
}

void map_row(char **str, int row) {
    char *ret = calloc(BUF_SIZE, 1);
    for(int i = 0; i < SIZEY; ++i) {
        char *buf;
        map_type_soldiers(&buf, row, i);
        int owner = map_owner(row, i);
        if(owner != -1)
            make_color(&buf, owner);
        if(row == POS_X && i == POS_Y)
            make_text_color(&buf);
        strcat(ret, buf);
        strcat(ret, " ");
    }
    *str = ret;
}

void str_action_mode(char **str) {
    char *ret = calloc(BUF_SIZE, 1);
    if(ACTION_MODE == ACTION_MODE_MOVE)
        strcpy(ret, "ACTION_MODE_MOVE");
    else if(ACTION_MODE == ACTION_MODE_SELECT_ALL)
        strcpy(ret, "ACTION_MODE_SELECT_ALL");
    else if(ACTION_MODE == ACTION_MODE_SELECT_HALF)
        strcpy(ret, "ACTION_MODE_SELECT_HALF");
    *str = ret;
}