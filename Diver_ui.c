#include <ncurses.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <locale.h>   // setlocale(), 로케일 설정
#include <wchar.h>

#define Noise 40
#define blink_time 5

#include "Diver_ui.h"
#include "global.h"
#include "store.h"

time_t start_time;

void draw_game_time() {
    time_t now = time(NULL);
    int elapsed = (int)(now - start_time);
    int minutes = elapsed / 60;
    int seconds = elapsed % 60;
    mvprintw(1, WIDTH - 10, "[%02d:%02d]", minutes, seconds);
}

void draw_border(int mode) {
    start_color(); // 색상 초기화

    // 초록색 & 빨간색 설정
    init_pair(1, COLOR_GREEN, COLOR_BLACK); // 공격 가능
    init_pair(2, COLOR_RED, COLOR_BLACK);   // 피격
    init_pair(3, COLOR_WHITE, COLOR_BLACK); // 공격 불가능 상황

    int color = (mode == 1) ? 1 : (mode == 2) ? 2 : 3;

    attron(COLOR_PAIR(color)); // 색상 적용
    box(stdscr, 0, 0);  // ✅ 테두리를 간결하게 `box()`로 구현
    attroff(COLOR_PAIR(color)); // 색상 해제

    refresh();
}

void size_check(){
    if (COLS != WIDTH || LINES != HEIGHT) {
        endwin();
        printf("\n\n🚨 DIVER : ONELIFE의 화면은 %dx%d으로 제작되었습니다. 터미널 크기를 %dx%d으로 설정하세요!\nstty size로 현재 터미널 크기를 확인할 수 있습니다.\n\n",WIDTH, HEIGHT, WIDTH, HEIGHT);
        exit(1);
    }
}

void reset_stat(){
    data = 200;
    atk_stat =20;
    dfs_stat = 20;
    pve_start_bit =3;
    pve_strong_atk_stat = 5.0 ;
    buy_atk_cnt =1;
    buy_dfs_cnt =1;
}


//screen
void start_screen(const char *client_name) {

    setlocale(LC_ALL, "ko_KR.UTF-8");
    initscr();

    noecho();
    curs_set(0);
    timeout(0); // non-blocking getch
    int blink = 1;
    int flip = 0;
    int ch;
    
    while (1) {
        clear();
        // 중앙 제목
        mvprintw(10, ((WIDTH - strlen("DIVER : ONE LIFE ONLINE")) / 2)-2, "DIVER : ONE LIFE ONLINE");

        int start_x = (COLS - WIDTH) / 2;
        int start_y = (LINES - HEIGHT) / 2;
        
        attron(A_BOLD);
        mvprintw(9, WIDTH / 2 - 22, "██████    ████   ██    ██   ██████   ███████ ");
        mvprintw(10,WIDTH / 2 - 22, "██   ██    ██    ██    ██   ██       ██    ██");
        mvprintw(11,WIDTH / 2 - 22, "██   ███   ██    ██    ██   ██████   ███████ ");
        mvprintw(12,WIDTH / 2 - 22, "██   ██    ██     ██  ██    ██       ██    ██");
        mvprintw(13,WIDTH / 2 - 22, "██████    ████     ████     ██████   ██     ██");
        mvprintw(16, ((WIDTH - strlen("ONE LIFE ONLINE")) / 2)-2, "ONE LIFE ONLINE");
        attroff(A_BOLD);






        // 환영 메시지
        char welcome[100];
        if (winning_streak >= 1) {
            snprintf(welcome, sizeof(welcome), "[%d연승] %s, 환영합니다!", winning_streak, client_name);
        } else {
            snprintf(welcome, sizeof(welcome), "%s, 환영합니다!", client_name);
        }
        mvprintw(18, (WIDTH - strlen(welcome)) / 2, "%s", welcome);

        // 깜빡이는 시작 메세지와 테두리
        if (blink>1) {attron(A_REVERSE);} // 🔥 반전 효과 적용
        mvprintw(19, (WIDTH - strlen("S를 눌러 시작하세요!")) / 2, "S를 눌러 시작하세요!");
        if (blink>1) {attroff(A_REVERSE);}// 🔥 효과 해제
        

        if(rand()%2){draw_border(3);}
        int x,y=0;
        // 0과 1 깜빡임
        for(int i =0; i<Noise;i++){
            x=rand()%(WIDTH - 2);
            y=rand()%(HEIGHT-2);
            if (!(y >= 9 && y <= 13 && x >= WIDTH / 2 - 22 && x <= WIDTH / 2 + 30)) {
                mvprintw(y, x, "%d", flip);
            }
            flip = !flip;
        }
        refresh();

        ch = getch();
        if (ch == 's' || ch == 'S'){ 
            start_time = time(NULL);
            break;}

        blink = (blink+1)%4;
        usleep(100000*blink_time);  // 0.1*blink_time초마다 토글
    }

    endwin();
}


//pause_screen
void pause_screen(int store_access1) {
    setlocale(LC_ALL, "ko_KR.UTF-8");
    initscr();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);

    clear();
    mvprintw(HEIGHT / 2 - 2, WIDTH / 2 - 10, "⏳ 일시 정지 화면");
    mvprintw(HEIGHT / 2 + 3, WIDTH/ 2 - 12, "아무 키나 눌러서 계속하세요...");
    draw_game_time();
    draw_border(3);
    refresh();
    int choice = pause_choice(store_access1); // 사용자 선택 받기
    clear();
    refresh();

    if (choice == 1) {
        return; // 다시 원래 화면으로 돌아가기
    } 
    else if (choice == 2) {
        store_menu_ui(); // 상점으로 이동 (store_status가 1일때)    
    }
    else if (choice == 3) {
        loser_ending_screen();
    }
    
}
int pause_choice(int store_access1) {
    const char *items[] = {
        "1. 다시 돌아가기",
        (store_access1 == 0) ? "❌ 지금은 상점에 진입할 수 없습니다." : "2. 상점으로 이동",
        "3. 프로그램 종료"
    };
    
    const int ITEM_COUNT = 3;  
    int highlight = 0; // 기본 선택값 (첫 번째 항목)

    while (1) {
        erase();
        draw_border(3);
        mvprintw(HEIGHT / 2 - 7, WIDTH / 2 - 10, "⏳ 일시 정지 화면");

        for (int i = 0; i < ITEM_COUNT; ++i) {
            if (i == highlight)
                attron(A_REVERSE); // 🔥 선택한 항목 반전 효과 적용

            mvprintw(HEIGHT / 2 + i -5, WIDTH / 2 - 8, "%s", items[i]);

            if (i == highlight)
                attroff(A_REVERSE); // 🔥 효과 해제
        }

        refresh();

        int ch = getch();
        if (ch == KEY_UP) {
            highlight = (highlight == 0) ? ITEM_COUNT - 1 : highlight - 1;}
        else if (ch == KEY_DOWN) {
            highlight = (highlight == ITEM_COUNT - 1) ? 0 : highlight + 1;} 
        else if (ch == '\n') {
            if (highlight != 1 || store_access1 == 1) { 
                return highlight +1;
            }}
    usleep(100000);
    }
     
}

void winner_ending_screen(){
    initscr();
    noecho();
    curs_set(0);
    start_color();  // 색상 활성화
    winning_streak+=1;
   
    
    attron(A_BOLD);
    mvprintw(HEIGHT / 2 - 3,WIDTH / 2 - 24, "██       ██  ████  ██    ██  ██    ██ ██████  ██████  ");
    mvprintw(HEIGHT / 2 - 2,WIDTH / 2 - 24, "██   ██  ██   ██   ████  ██  ████  ██ ██      ██   ██ ");
    mvprintw(HEIGHT / 2 - 1,WIDTH / 2 - 24, "██  ███  ██   ██   ██ ██ ██  ██ ██ ██ ██████  ██████  ");
    mvprintw(HEIGHT / 2 + 0,WIDTH / 2 - 24, "██ ██ ██ ██   ██   ██  ████  ██  ████ ██      ██   ██ ");
    mvprintw(HEIGHT / 2 + 1,WIDTH / 2 - 24, "████   ████  ████  ██   ███  ██   ███ ██████  ██    ██");
    attroff(A_BOLD);
    mvprintw(HEIGHT / 2 + 4, WIDTH / 2 - 10, "Preparing NextGame..");

    char congratulations[100];
    if (winning_streak > 1) {
        snprintf(congratulations, sizeof(congratulations), "%d번째 승리 축하드립니다!", winning_streak);
    } else {
        snprintf(congratulations, sizeof(congratulations), "첫번째 승리 축하드립니다!");
    }
    mvprintw(HEIGHT / 2 + 5, (WIDTH - strlen(congratulations)) / 2, "%s", congratulations);
    

    refresh();
    sleep(3);

    endwin();
    printf("🛑 당신은 승리했습니다.\n");
    _exit(0);


}

void loser_ending_screen(){
    initscr();
    noecho();
    curs_set(0);
    start_color();  // 색상 활성화

    attron(A_BOLD);
    mvprintw(HEIGHT / 2 - 3,WIDTH / 2 - 20, "██        ████████  ███████  ██████   ██████  ");
    mvprintw(HEIGHT / 2 - 2,WIDTH / 2 - 20, "██        ██    ██  ██       ██       ██   ██ ");
    mvprintw(HEIGHT / 2 - 1,WIDTH / 2 - 20, "██        ██    ██    ████   ██████   ██████  ");
    mvprintw(HEIGHT / 2 + 0,WIDTH / 2 - 20, "██        ██    ██       ██  ██       ██   ██ ");
    mvprintw(HEIGHT / 2 + 1,WIDTH / 2 - 20, "████████  ████████  ███████  ██████   ██    ██");
    attroff(A_BOLD);
    mvprintw(HEIGHT / 2 + 4,WIDTH / 2 - 10, "DELETE EVERYTHING...");

    refresh();
    sleep(3);

    endwin();
    printf("🛑 당신은 패배했습니다.\n");
    _exit(0);


}