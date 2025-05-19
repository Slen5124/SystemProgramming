#include <ncurses.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>
#include <wchar.h>
#include "Diver_ui.h"
#include "global.h"

#define Noise 25
#define blink_time 5

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

        // 환영 메시지
        char welcome[100];
        snprintf(welcome, sizeof(welcome), "%s, 환영합니다!", client_name);
        mvprintw(12, (WIDTH - strlen(welcome)) / 2, "%s", welcome);

        // 깜빡이는 시작 메세지와 테두리
        if (blink)
            mvprintw(14, (WIDTH - strlen("S를 눌러 시작하세요!")) / 2, "S를 눌러 시작하세요!");
        
        if(rand()%2){draw_border(3);}
        
        // 0과 1 깜빡임
        for(int i =0; i<Noise;i++){
            mvprintw(rand()%(HEIGHT-2), rand()%(WIDTH - 2), "%d", flip);
            flip = !flip;
        }
        refresh();

        ch = getch();
        if (ch == 's' || ch == 'S') break;

        blink = !blink;
        usleep(100000*blink_time);  // 0.1*blink_time초마다 토글
    }

    endwin();
}
