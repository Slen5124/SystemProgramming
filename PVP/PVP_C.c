// PVP_C.c – JSON 기반 클라이언트 (130×35 레이아웃 기준)
// 🎶 웅장한 승패 화면, 자동 터미널 복구까지! raw 모드+XON/XOFF로 Ctrl 키 캡처

#define _XOPEN_SOURCE_EXTENDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <locale.h>
#include <signal.h>
#include <termios.h>            // IXON/IXOFF 제어
#include <ncursesw/ncurses.h>   // wide-char ncurses (def_shell_mode 등)
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdbool.h>

#include "shared_eco.h"         // get_current_time_ms(), DELAY_*, PlayerState 등
#include "json_topic.h"         // JSON 파싱/생성 함수

#define DEFAULT_SERVER_IP   "172.20.12.161"
#define PORT                5124
#define BUF_SIZE            1024
#define LOG_LINES           5
#define MAX_DATA            100
#define FILLED_CHR          "█"
#define EMPTY_CHR           " "
#define BAR_LEN             20



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
        // UTF-8 -> wchar_t 변환
        wchar_t wbuf[128];
        mbstowcs(wbuf, art[i], 128);
        int len = wcslen(wbuf);
        // 좌우 반전
        for (int j = 0; j < len/2; j++) {
            wchar_t tmp = wbuf[j];
            wbuf[j] = wbuf[len-1-j];
            wbuf[len-1-j] = tmp;
        }
        // 다시 멀티바이트로
        char mbbuf[256];
        wcstombs(mbbuf, wbuf, 256);
        // 92열에서 출력
        mvprintw(7 + i, 91, "%s", mbbuf);
    }
}


// ─ 전역 상태 변수 ───────────────────────────────────────────
static char     logs[LOG_LINES][BUF_SIZE];
static int      logi           = 0;
static char     opponent_name[32] = "???";
static int      my_data   = MAX_DATA,   my_charge = 0;
static int      en_data   = MAX_DATA,   en_charge = 0;
static long long delay_until = 0;
static int       in_delay     = 0;

// ─ 웅장한 승패 화면 ─────────────────────────────────────────
void draw_grand_result(int win) {
    int y, x;
    getmaxyx(stdscr, y, x);
    clear();
    if      (win > 0)  attron(COLOR_PAIR(5)); // 승리
    else if (win == 0) attron(COLOR_PAIR(6)); // 무승부
    else               attron(COLOR_PAIR(7)); // 패배
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

// ─ 로그 함수 ─────────────────────────────────────────────────
void add_log(const char *msg) {
    strncpy(logs[logi%LOG_LINES], msg, BUF_SIZE-1);
    logs[logi%LOG_LINES][BUF_SIZE-1] = '\0';
    logi++;
}
void draw_logs() {
    int start = logi>LOG_LINES ? logi-LOG_LINES : 0;
    for(int i=0;i<LOG_LINES;i++){
        mvprintw(28+i, 4, "%s", logs[(start+i)%LOG_LINES]);
    }
}

// ─ 상태 바 그리기 ───────────────────────────────────────────
void draw_bar(int y,int x,int val,const char *lab){
    int v = val<0?0:(val>MAX_DATA?MAX_DATA:val);
    int f = v*BAR_LEN/MAX_DATA;
    mvprintw(y, x, "%-8s ", lab);
    for(int i=0;i<BAR_LEN;i++) printw(i<f?FILLED_CHR:EMPTY_CHR);
    printw(" %3d/%3d", v, MAX_DATA);
}
void draw_status(const char *nick) {
    mvprintw(1,2,  "🧑 You:   %-12s", nick);
    draw_bar(3,2,my_data,"DATA");
    draw_bar(5,2,my_charge,"⚡CHARGE");
    mvprintw(1,90,"👾Enemy:  %-12s", opponent_name);
    draw_bar(3,90,en_data,"DATA");
    draw_bar(5,90,en_charge,"⚡CHARGE");
    draw_dot_art();
    draw_dot_art_enemy();
}

// ─ 컨트롤 안내 ─────────────────────────────────────────────
void draw_ctrl() {
    mvprintw(28, 90, "[Ctrl+X] ATTACK   [Ctrl+Z] BLOCK");
    mvprintw(30, 90, "[Ctrl+C] Charge×3 [Ctrl+A] Charge×5");
    mvprintw(32, 90, "[Ctrl+S] COUNTER   [q] Quit");
}

// ─ 메인 ────────────────────────────────────────────────────
int main(int argc, char *argv[]) {
    def_shell_mode();

    setlocale(LC_ALL,"");
    signal(SIGINT, SIG_IGN);
    signal(SIGTSTP,SIG_IGN);

    // XON/XOFF 해제
    struct termios tt; tcgetattr(0,&tt);
    tt.c_iflag&=~(IXON|IXOFF); tcsetattr(0,TCSANOW,&tt);

    // 서버 IP 인자 처리
    const char *server_ip = (argc>=2 ? argv[1] : DEFAULT_SERVER_IP);

    // 닉네임
    char nick[32]; printf("Enter your nickname: "); scanf("%31s",nick);

    // 서버 연결
    int sock = socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in srv={.sin_family=AF_INET,.sin_port=htons(PORT)};
    if(inet_pton(AF_INET,server_ip,&srv.sin_addr)!=1){
        perror("inet_pton"); exit(1);
    }
    if(connect(sock,(struct sockaddr*)&srv,sizeof(srv))<0){
        perror("connect"); exit(1);
    }

    // REGISTER
    char buf[BUF_SIZE]; long long ts=get_current_time_ms();
    snprintf(buf,BUF_SIZE,
        "{\"action\":\"REGISTER\",\"nickname\":\"%s\",\"timestamp\":%lld}",
        nick,ts);
    send(sock,buf,strlen(buf),0);

    // “Game Started” 수신 & 상대 닉네임 파싱
    while(recv(sock,buf,BUF_SIZE-1,0)<=0);
    add_log("Game Started");
    parse_nickname_from_json(buf, opponent_name, sizeof(opponent_name));

    // ncurses 초기화
    initscr(); def_prog_mode();
    raw(); noecho(); keypad(stdscr,TRUE);
    nodelay(stdscr,TRUE); timeout(0); curs_set(0);

    // 색상 페어
    start_color();
    init_pair(1, COLOR_RED,    COLOR_BLACK);
    init_pair(2, COLOR_GREEN,  COLOR_BLACK);
    init_pair(5, COLOR_YELLOW, COLOR_BLUE);
    init_pair(6, COLOR_WHITE,  COLOR_MAGENTA);
    init_pair(7, COLOR_WHITE,  COLOR_RED);

    // 메인 루프 (100Hz)
    while(1){
        long long now = get_current_time_ms();
        if(in_delay && now>=delay_until) in_delay=0;

        int ch=getch(), sent=0;
        if      (ch==24){ ts=now; in_delay=1; delay_until=ts+DELAY_ATTACK;
            snprintf(buf,BUF_SIZE,
                "{\"action\":\"ATTACK\",\"nickname\":\"%s\",\"timestamp\":%lld}",
                nick,ts); sent=1;
        } else if (ch==26){ ts=now; in_delay=1; delay_until=ts+DELAY_BLOCK;
            snprintf(buf,BUF_SIZE,
                "{\"action\":\"BLOCK\",\"nickname\":\"%s\",\"timestamp\":%lld}",
                nick,ts); sent=1;
        } else if (ch==3){ ts=now; in_delay=1; delay_until=ts+DELAY_CHARGE_WEAK;
            snprintf(buf,BUF_SIZE,
                "{\"action\":\"CHARGE_WEAK\",\"nickname\":\"%s\",\"timestamp\":%lld}",
                nick,ts); sent=1;
        } else if (ch==1){ ts=now; in_delay=1; delay_until=ts+DELAY_CHARGE_STRONG;
            snprintf(buf,BUF_SIZE,
                "{\"action\":\"CHARGE_STRONG\",\"nickname\":\"%s\",\"timestamp\":%lld}",
                nick,ts); sent=1;
        } else if (ch==19){ ts=now; in_delay=1; delay_until=ts+COUNTER_WINDOW;
            snprintf(buf,BUF_SIZE,
                "{\"action\":\"COUNTER\",\"nickname\":\"%s\",\"timestamp\":%lld}",
                nick,ts); sent=1;
        } else if (ch=='q') break;
        if(sent) send(sock,buf,strlen(buf),0);

        // 서버 응답
        // (원래 있던 위치에 덮어쓰기)
        int n = recv(sock, buf, BUF_SIZE-1, MSG_DONTWAIT);
        if (n > 0) {
            buf[n] = '\0';

            // 1) 상대 닉네임 갱신 (필요하면 여기서)
            parse_nickname_from_json(buf, opponent_name, sizeof(opponent_name));

            // 2) event 메시지만 연속으로 찾아서 처리
            char *p = buf;
            while (1) {
                char *ev = strstr(p, "\"event\":\"");
                if (!ev) break;
                ev += strlen("\"event\":\"");
                char *e1 = strchr(ev, '"');
                if (!e1) break;
                *e1 = '\0';

                // 로그에도 남기고
                add_log(ev);

                // Game Over 감지
                if (strstr(ev, "Game Over")) {
                    int result = strstr(ev, "You Win")   ? 1
                                : strstr(ev, "You Lose") ? -1
                                                        : 0;
                    draw_grand_result(result);
                }

                // 다음 검색 위치는 방금 닫은 따옴표 뒤
                p = e1 + 1;
            }

            // 3) self/opponent 상태는 마지막 JSON 블록에서 뽑아주세요
            //    (필요하다면 위 루프 안쪽에서도 매번 갱신해도 무방합니다)
            char *s0 = strstr(buf, "\"self\":");
            if (s0) sscanf(s0, "\"self\":{\"data\":%d,\"charged_attack\":%d}", &my_data, &my_charge);
            char *s1 = strstr(buf, "\"opponent\":");
            if (s1) sscanf(s1, "\"opponent\":{\"data\":%d,\"charged_attack\":%d}", &en_data, &en_charge);
        }


        // 화면 업데이트
        erase();
        if(in_delay) attron(COLOR_PAIR(1)); else attron(COLOR_PAIR(2));
        box(stdscr,0,0);
        if(in_delay) attroff(COLOR_PAIR(1)); else attroff(COLOR_PAIR(2));

        draw_status(nick);
        draw_ctrl();
        draw_logs();
        refresh();
        usleep(10000);
    }

    // 종료 복귀
    endwin();
    reset_shell_mode();
    system("clear");
    close(sock);
    return 0;
}
