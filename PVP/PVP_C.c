#define _XOPEN_SOURCE_EXTENDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <locale.h>
#include <signal.h>
#include <termios.h>
#include <ncursesw/ncurses.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <wchar.h>
#include <time.h>

#include "../global.h"
#include "../Diver_ui.h"
#include "json_topic.h"

#define BUF_SIZE    1024
#define LOG_LINES   5
#define MAX_DATA    200
#define FILLED_CHR  "█"
#define EMPTY_CHR   " "
#define BAR_LEN     20

static char logs[LOG_LINES][BUF_SIZE];
static int  logi = 0;
static char opponent_name[32] = "???";

void add_log(const char *msg) {
    strncpy(logs[logi % LOG_LINES], msg, BUF_SIZE - 1);
    logs[logi % LOG_LINES][BUF_SIZE - 1] = '\0';
    logi++;
}

void draw_logs() {
    int start = logi > LOG_LINES ? logi - LOG_LINES : 0;
    for (int i = 0; i < LOG_LINES; i++)
        mvprintw(28 + i, 4, "%s", logs[(start + i) % LOG_LINES]);
}

void draw_bar(int y, int x, int val, int max_val, const char *lab){
    int v = val<0?0:(val>max_val?max_val:val);
    int f = v*BAR_LEN/max_val;
    mvprintw(y, x, "%-8s ", lab);
    for(int i=0;i<BAR_LEN;i++) printw(i<f?FILLED_CHR:EMPTY_CHR);
    printw(" %3d/%3d", v, max_val);
}

void draw_ctrl() {
    mvprintw(28, 90, "[Ctrl+X] ATTACK   [Ctrl+Z] BLOCK");
    mvprintw(30, 90, "[Ctrl+C] Charge×3 [Ctrl+A] Charge×5");
    mvprintw(32, 90, "[Ctrl+S] COUNTER   [q] Quit");
}

void draw_dot_art() {
    const char *art[] = {
        "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⣀⣤⣠⣄⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀",
        "⠀⠀⠀⠀⠀⠀⠀⠀⢠⣞⡷⣻⢞⡳⣏⢾⡹⣖⡄⠀⠀⠀⠀⠀⠀",
        "⠀⠀⠀⠀⠀⠀⠀⠀⢾⣱⢏⡷⢫⠵⣩⠎⡵⠈⠜⠀⠀⠀⠀⠀⠀",
        "⠀⠀⠀⠀⠀⠀⠀⠀⢻⣜⢯⡜⣣⢣⡑⢎⠐⣁⠦⠀⠀⠀⠀⠀⠀",
        "⠀⠀⠀⠀⠀⠀⠀⠀⠘⣮⢳⡹⢰⡐⠄⠢⢄⢨⠁⡀⠀⠀⠀⠀⠀",
        "⠀⠀⠀⠀⠀⠀⠀⠀⠀⣇⢃⠒⡠⠙⣂⠡⢌⢂⡑⠀⠀⠀⠀⠀⠀",
        "⠀⠀⠀⠀⠀⠀⠀⢀⡸⢀⠣⢈⠰⡀⢼⠳⣌⠢⠀⠀⠀⠀⠀⠀⠀",
        "⠀⠀⠀⠀⠀⢠⢢⠑⠂⡌⠄⠂⠑⠎⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀",
        "⠀⠀⠀⠀⡠⠌⡀⠐⡀⢌⡑⠠⢈⠐⠠⠈⡐⠠⠠⠀⠀⠀⠀⠀⠀",
        "⠀⢠⢣⡑⢆⠐⡀⠐⡆⢄⠣⠐⠨⠐⠠⠀⠐⠀⠂⠈⢄⠀⠀⠀⠀",
        "⠀⣧⠒⡜⣌⠠⣀⠹⡐⢌⠆⠡⢈⠁⢆⠁⠌⠐⠀⠡⢈⠄⠀⠀⠀",
        "⠀⣧⢋⠴⡨⠔⣀⠳⡈⢞⠤⢁⠂⠌⡀⢊⠔⡡⢈⠐⢠⠊⠀⠀⠀",
        "⠀⢧⣋⢖⡡⢫⠄⣣⠑⢎⡖⡈⢌⠠⡑⡌⣞⠡⢂⠜⡠⠃⠀⠀⠀",
        "⠘⢦⡙⣮⡑⡣⢎⡱⣉⠆⡙⢦⣈⠱⡐⡡⢏⡔⠀⢂⠡⡁⠀⠀⠀",
        "⠘⢦⡙⢦⡙⢶⢡⢚⡥⢊⡱⠌⢦⠱⣠⠣⣏⠔⡉⢀⠢⣁⠀⠀⠀",
        "⠈⢦⡙⢮⡝⢮⡑⡎⡵⣃⠖⡩⢆⡱⢠⢛⡼⣊⠔⡁⢢⠐⠀⠀⠀",
        "⠀⠧⣜⢣⠞⣥⢛⡬⡱⣏⢎⡱⢌⠲⣡⢏⡾⣱⢊⠔⡠⢍⠀⠀⠀",
        "⠀⠳⣌⢧⢫⠀⣏⠶⡑⣎⢮⢱⣊⠕⡢⢏⡾⡱⢫⠄⡱⢌⢂⠀⠀",
        "⠀⠘⡬⢎⡭⢳⡌⢳⡑⣎⢆⠣⢌⢊⠱⣩⢞⡥⣛⠼⣠⢉⠲⡀⠀"
    };
    for (int i = 0; i < sizeof(art)/sizeof(art[0]); i++) {
        mvprintw(7 + i, 15, "%s", art[i]);
    }
}

void draw_dot_art_enemy() {
    const char *art[] = {
        "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⣀⣤⣠⣄⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀",
        "⠀⠀⠀⠀⠀⠀⠀⠀⢠⣞⡷⣻⢞⡳⣏⢾⡹⣖⡄⠀⠀⠀⠀⠀⠀",
        "⠀⠀⠀⠀⠀⠀⠀⠀⢾⣱⢏⡷⢫⠵⣩⠎⡵⠈⠜⠀⠀⠀⠀⠀⠀",
        "⠀⠀⠀⠀⠀⠀⠀⠀⢻⣜⢯⡜⣣⢣⡑⢎⠐⣁⠦⠀⠀⠀⠀⠀⠀",
        "⠀⠀⠀⠀⠀⠀⠀⠀⠘⣮⢳⡹⢰⡐⠄⠢⢄⢨⠁⡀⠀⠀⠀⠀⠀",
        "⠀⠀⠀⠀⠀⠀⠀⠀⠀⣇⢃⠒⡠⠙⣂⠡⢌⢂⡑⠀⠀⠀⠀⠀⠀",
        "⠀⠀⠀⠀⠀⠀⠀⢀⡸⢀⠣⢈⠰⡀⢼⠳⣌⠢⠀⠀⠀⠀⠀⠀⠀",
        "⠀⠀⠀⠀⠀⢠⢢⠑⠂⡌⠄⠂⠑⠎⠀⠀⠀⠀⠀⠀⠀⠀⠀  ⠀",
        "⠀⠀⠀⠀⡠⠌⡀⠐⡀⢌⡑⠠⢈⠐⠠⠈⡐⠠⠠⠀⠀⠀⠀⠀⠀",
        "⠀⢠⢣⡑⢆⠐⡀⠐⡆⢄⠣⠐⠨⠐⠠⠀⠐⠀⠂⠈⢄⠀⠀⠀⠀",
        "⠀⣧⠒⡜⣌⠠⣀⠹⡐⢌⠆⠡⢈⠁⢆⠁⠌⠐⠀⠡⢈⠄⠀⠀⠀",
        "⠀⣧⢋⠴⡨⠔⣀⠳⡈⢞⠤⢁⠂⠌⡀⢊⠔⡡⢈⠐⢠⠊⠀⠀⠀",
        "⠀⢧⣋⢖⡡⢫⠄⣣⠑⢎⡖⡈⢌⠠⡑⡌⣞⠡⢂⠜⡠⠃⠀⠀⠀",
        "⠘⢦⡙⣮⡑⡣⢎⡱⣉⠆⡙⢦⣈⠱⡐⡡⢏⡔⠀⢂⠡⡁⠀⠀⠀",
        "⠘⢦⡙⢦⡙⢶⢡⢚⡥⢊⡱⠌⢦⠱⣠⠣⣏⠔⡉⢀⠢⣁⠀⠀⠀",
        "⠈⢦⡙⢮⡝⢮⡑⡎⡵⣃⠖⡩⢆⡱⢠⢛⡼⣊⠔⡁⢢⠐⠀⠀⠀",
        "⠀⠧⣜⢣⠞⣥⢛⡬⡱⣏⢎⡱⢌⠲⣡⢏⡾⣱⢊⠔⡠⢍⠀⠀⠀",
        "⠀⠳⣌⢧⢫⠀⣏⠶⡑⣎⢮⢱⣊⠕⡢⢏⡾⡱⢫⠄⡱⢌⢂⠀⠀",
        "⠀⠘⡬⢎⡭⢳⡌⢳⡑⣎⢆⠣⢌⢊⠱⣩⢞⡥⣛⠼⣠⢉⠲⡀⠀"
    };
    int lines = sizeof(art)/sizeof(art[0]);
    setlocale(LC_ALL, "");
    for (int i = 0; i < lines; i++) {
        wchar_t wbuf[128];
        mbstowcs(wbuf, art[i], 128);
        int len = wcslen(wbuf);
        for (int j = 0; j < len/2; j++) {
            wchar_t tmp = wbuf[j];
            wbuf[j] = wbuf[len-1-j];
            wbuf[len-1-j] = tmp;
        }
        char mbbuf[256];
        wcstombs(mbbuf, wbuf, 256);
        mvprintw(7 + i, 91, "%s", mbbuf);
    }
}

void draw_status(
    const char* nick,
    int my_data, int my_max_data, int my_charge,
    const char* opponent_name,
    int en_data, int en_max_data, int en_charge
) {
    mvprintw(1, 2,  "🧑 You:   %-12s", nick);
    draw_bar(3, 2, my_data, my_max_data, "DATA");
    draw_bar(5, 2, my_charge, 200, "⚡CHARGE");
    mvprintw(1, 90, "👾Enemy:  %-12s", opponent_name);
    draw_bar(3, 90, en_data, en_max_data, "DATA");
    draw_bar(5, 90, en_charge, 200, "⚡CHARGE");
    draw_dot_art();
    draw_dot_art_enemy();
}

void draw_grand_result(int win) {
    int y, x;
    getmaxyx(stdscr, y, x);
    clear();
    if      (win > 0)  attron(COLOR_PAIR(5));
    else if (win == 0) attron(COLOR_PAIR(6));
    else               attron(COLOR_PAIR(7));
    for (int i = 0; i < y; i++) mvhline(i, 0, ' ', x);
    const char *banner[] = {
        "..######..##....##.########.##.......########..######.",
        ".##....##.##...##..##.......##.......##.......##....##",
        ".##.......##..##...##.......##.......##.......##......",
        ".##.......#####....######...##.......######...##......",
        ".##.......##..##...##.......##.......##.......##......",
        ".##....##.##...##..##.......##.......##.......##....##",
        "..######..##....##.########.########.########..######."
    };
    int lines  = sizeof(banner)/sizeof(banner[0]);
    int starty = (y - lines)/2 - 2;
    attron(A_BOLD);
    for(int i=0;i<lines;i++){
        mvprintw(starty+i, (x-strlen(banner[i]))/2, "%s", banner[i]);
    }
    attroff(A_BOLD);
    const char *msg = win>0
        ? "🎉 VICTORY! Press 'q' to quit 🎉"
        : win==0
        ? "🤝 IT'S A DRAW! Press 'q' to quit 🤝"
        : "💀 DEFEATED! Press 'q' to quit 💀";
    attron(A_UNDERLINE|A_BOLD);
    mvprintw(starty+lines+2, (x-strlen(msg))/2, "%s", msg);
    attroff(A_UNDERLINE|A_BOLD);

    refresh();
    nodelay(stdscr, FALSE);
    while(getch()!='q');
    endwin();
    reset_shell_mode();
    system("clear");
    exit(0);
}

int run_pvp_mode(int sock) {
    def_shell_mode();
    setlocale(LC_ALL,"");

    // REGISTER(내 능력치 포함, nick, atk_stat, dfs_stat)
    char buf[BUF_SIZE];
    long long ts = get_current_time_ms();
    snprintf(buf, BUF_SIZE,
        "{\"action\":\"REGISTER\",\"nickname\":\"%s\",\"data\":%d,\"max_data\":%d,\"atk\":%d,\"dfs\":%d,\"timestamp\":%lld}",
        Player.nick, Player.data, Player.max_data, Player.atk_stat, Player.dfs_stat, ts);
    send(sock, buf, strlen(buf), 0);

    // 상대방 접속 대기 (웨이팅/로딩 애니메이션)
    initscr(); def_prog_mode();
    raw(); noecho(); keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE); timeout(0); curs_set(0);
    start_color();
    init_pair(1, COLOR_RED,    COLOR_BLACK);
    init_pair(2, COLOR_GREEN,  COLOR_BLACK);
    init_pair(5, COLOR_YELLOW, COLOR_BLUE);
    init_pair(6, COLOR_WHITE,  COLOR_MAGENTA);
    init_pair(7, COLOR_WHITE,  COLOR_RED);

    while(recv(sock,buf,BUF_SIZE-1,0)<=0);
    add_log("Game Started");
    parse_nickname_from_json(buf, opponent_name, sizeof(opponent_name));


    int my_data = Player.data, my_max_data = Player.max_data, my_charge = 0;
    int en_data = Player.data, en_max_data = Player.max_data, en_charge = 0;

    long long delay_until = 0;
    int in_delay = 0;

    // 본게임 루프
    while(1){
        long long now = get_current_time_ms();
        if(in_delay && now >= delay_until) in_delay=0;

        int ch = getch(), sent = 0;
        if      (ch==24){ ts=now; in_delay=1; delay_until=ts+DELAY_ATTACK;
            snprintf(buf,BUF_SIZE,
                "{\"action\":\"ATTACK\",\"nickname\":\"%s\",\"timestamp\":%lld}",
                Player.nick, ts); sent=1;
        } else if (ch==26){ ts=now; in_delay=1; delay_until=ts+DELAY_BLOCK;
            snprintf(buf,BUF_SIZE,
                "{\"action\":\"BLOCK\",\"nickname\":\"%s\",\"timestamp\":%lld}",
                Player.nick, ts); sent=1;
        } else if (ch==3){ ts=now; in_delay=1; delay_until=ts+DELAY_CHARGE_WEAK;
            snprintf(buf,BUF_SIZE,
                "{\"action\":\"CHARGE_WEAK\",\"nickname\":\"%s\",\"timestamp\":%lld}",
                Player.nick, ts); sent=1;
        } else if (ch==1){ ts=now; in_delay=1; delay_until=ts+DELAY_CHARGE_STRONG;
            snprintf(buf,BUF_SIZE,
                "{\"action\":\"CHARGE_STRONG\",\"nickname\":\"%s\",\"timestamp\":%lld}",
                Player.nick, ts); sent=1;
        } else if (ch==19){ ts=now; in_delay=1; delay_until=ts+COUNTER_WINDOW;
            snprintf(buf,BUF_SIZE,
                "{\"action\":\"COUNTER\",\"nickname\":\"%s\",\"timestamp\":%lld}",
                Player.nick, ts); sent=1;
        } else if (ch=='q') break;
        if(sent) send(sock, buf, strlen(buf), 0);

        // 서버 응답 파싱
        int n = recv(sock, buf, BUF_SIZE-1, MSG_DONTWAIT);
        if (n > 0) {
            buf[n] = '\0';
            char *s0 = strstr(buf, "\"self\":");
            char *s1 = strstr(buf, "\"opponent\":");
            if (s0) sscanf(s0, "\"self\":{\"data\":%d,\"max_data\":%d,\"charged_attack\":%d,\"nickname\":\"%*31[^\"]\"}", &my_data, &my_max_data, &my_charge);
            //              ^ nickname 파싱은 무시 (덮어쓰기 방지)
            if (s1) sscanf(s1, "\"opponent\":{\"data\":%d,\"max_data\":%d,\"charged_attack\":%d,\"nickname\":\"%31[^\"]\"}", &en_data, &en_max_data, &en_charge, opponent_name);

            // 이벤트 파싱 및 로그
            char *p = buf;
            while (1) {
                char *ev = strstr(p, "\"event\":\"");
                if (!ev) break;
                ev += strlen("\"event\":\"");
                char *e1 = strchr(ev, '"');
                if (!e1) break;
                *e1 = '\0';
                add_log(ev);

                if (strstr(ev, "Game Over")) {
                    int result = strstr(ev, "You Win")   ? 1
                                : strstr(ev, "You Lose") ? -1
                                                        : 0;
                    draw_grand_result(result);
                }
                p = e1 + 1;
            }
        }

        // 화면 갱신
        erase();
        if(in_delay) attron(COLOR_PAIR(1)); else attron(COLOR_PAIR(2));
        box(stdscr,0,0);
        if(in_delay) attroff(COLOR_PAIR(1)); else attroff(COLOR_PAIR(2));
        draw_status(Player.nick,
            my_data, my_max_data, my_charge,
            opponent_name,
            en_data, en_max_data, en_charge);
        draw_ctrl();
        draw_logs();
        refresh();
        usleep(10000);
    }

    // 종료 복귀
    endwin();
    reset_shell_mode();
    system("clear");
    return 0;
}
