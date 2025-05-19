#include <ncurses.h>
#include <unistd.h>
#include <time.h>
#include <locale.h>
#include "Diver_ui.h"
#include "store.h"
#include "global.h"

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
        endwin();
        printf("🛑 당신은 패배했습니다.\n");
        _exit(0);
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
