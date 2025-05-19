#include <ncurses.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <locale.h>   // setlocale(), 로케일 설정
#include "Diver_ui.h"
#include "global.h"

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
    init_pair(1, COLOR_GREEN, COLOR_BLACK); //공격가능
    init_pair(2, COLOR_RED, COLOR_BLACK);  //피격
    init_pair(3, COLOR_WHITE, COLOR_BLACK); //공격불가능상황
    int color;
    if (mode == 1) {
        color = 1;  
    } else if (mode == 2) {
        color = 2;  
    } else {
        color = 3;  
    }

    attron(COLOR_PAIR(color)); // 색상 적용

    // 테두리 그리기 (문자 '#' 사용)
    for (int i = 0; i < WIDTH; i++) {
        mvprintw(0, i, "#");
        mvprintw(HEIGHT - 1, i, "#");
    }
    for (int i = 0; i < HEIGHT; i++) {
        mvprintw(i, 0, "#");
        mvprintw(i, WIDTH - 1, "#");
    }

    attroff(COLOR_PAIR(color)); // 색상 해제
    refresh();
}

