// store.c
#include <ncurses.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <locale.h>   // setlocale(), 로케일 설정
#include "store.h"
#include "Diver_ui.h"
#include "global.h"
#include "log.h"

#define options_COUNT 4
#define Special_ablity_COUNT 6
#define PVE_ablity_COUNT 3
#define Time_Start 100

const char *options[options_COUNT] = { "1. 강화 - 공격력 +10", "2. 강화 - 방어력 +10", "3. 아이템구매","4. 나가기" };
int cnt =0;
const char *Special_ablity[Special_ablity_COUNT] = {"시작  Bit", 
"Data 수급량",
"강한 공격 계수",
"차지  속도",
"카운터 공격 계수", 
"강한차지 충전량"
 };
const char *Special_ablity_coefficient[Special_ablity_COUNT] = { "  +1", "x 1.5배", "x 1.2배",
                                                        " -0.5s","x 1.2배", "x 1.2배" };


void call_store(){ 
    int choice =1;
    rand_ability_no_dup();
    while(choice!=3){
        choice = store_menu_ui(); // 상점 기능 호출
        initscr();
        handle_upgrade(choice);
    }
}

void handle_upgrade(int choice) {
    if (choice == 0) {
        if(data>10*buy_atk_cnt){
           data-=10*buy_atk_cnt++;
            atk_stat +=10;
            write_log_file("공격력 강화","upgrade_log.txt");}
        else{write_log_file("구매가 불가능합니다","upgrade_log.txt");}
    } 
    else if (choice == 1) {
        if(data>10*buy_dfs_cnt){
            data-=10*buy_dfs_cnt++;
            dfs_stat +=10;
            write_log_file("방어력 강화","upgrade_log.txt");}
        else{write_log_file("구매가 불가능합니다","upgrade_log.txt");}
    }
    else if (choice == 2) {
        if(data>50){
            data-=50;
            write_log_file("특수능력 구매","upgrade_log.txt");}
        else{write_log_file("구매가 불가능합니다","upgrade_log.txt");}
    }    
    else if(choice ==3){
        mvprintw(LINES - 2, 1, "상점에서 나갔습니다.");
    }
    refresh();
}

void rand_ability_no_dup(){
    do{
    ability_sort = rand()%Special_ablity_COUNT;
    }while(ability_dup_check[ability_sort]);
}

void draw_store_ui(int highlight, int time_left) {
    setlocale(LC_ALL, "ko_KR.utf-8");
    if(!cnt)erase();if(cnt<1)cnt++;
    draw_border(3);  // erase() 제거로 화면 전체 초기화 방지
    draw_game_time();
    
    // ✅ 1사분면 (플레이어 정보) 오른쪽 5칸
    mvprintw(2, WIDTH / 2, "[플레이어 정보]");
    mvprintw(4, WIDTH / 2, "데이터: %4d", data);
    mvprintw(5, WIDTH / 2, "공격력: %4d", atk_stat);
    mvprintw(6, WIDTH / 2, "방어력: %4d", dfs_stat);
    mvprintw(7, WIDTH / 2, "PVE 시작 비트: %d", pve_start_bit);
    mvprintw(8, WIDTH / 2, "강력 공격 배율: %.1f", pve_strong_atk_stat);

    
    // ✅ 2사분면 (상점 상품)
    mvprintw(2, 2, "[상점 상품]");
     
    // 🔥 가로 3칸 구분, 세로 간격 늘리기
    mvprintw(4, 2,  "------------------------------------------------------------");
    mvprintw(5, 2,  "|                  |                  |                    |");
    mvprintw(6, 2,  "| 공격력 +10 강화  | 방어력 +10 강화  |    특수능력 구매   |");
    mvprintw(7, 2,  "|                  |                  |       [%s]        |", ability_sort<3?"PVE":"PVP");
    mvprintw(8, 2,  "|                  |                  |                    |");
    mvprintw(9, 2, "|                  |                  |    %s",Special_ablity[ability_sort]); 
    mvprintw(9, 61,"|");
    mvprintw(10, 2, "|                  |                  |       %s",Special_ablity_coefficient[ability_sort]);
    mvprintw(10, 61,"|");
    
    
    
    
    mvprintw(11, 2, "|                  |                  |                    |");
    mvprintw(12, 2, "| 필요 데이터: %d  | 필요 데이터: %d  |     50 데이터      |", 10 * buy_atk_cnt, 10 * buy_dfs_cnt);
    mvprintw(13, 2, "------------------------------------------------------------");

    // ✅ 3사분면 (로그) 아래로 5칸
    int log_y_offset = HEIGHT / 2;
    mvprintw(log_y_offset, 2, "[업그레이드 내역]");
    read_log_file("upgrade_log.txt", log_y_offset,2);


    // ✅ 4사분면 (선택 메뉴) → 오른쪽 5칸, 아래 5줄
    int select_y_offset = HEIGHT / 2;
    int select_x_offset = WIDTH / 2 - 5 + 5;

    mvprintw(select_y_offset, select_x_offset, "[상점 선택]");
    for (int i = 0; i < options_COUNT; ++i) {
        if (i == highlight) attron(A_REVERSE);
        mvprintw(select_y_offset + 2 + i, select_x_offset, "%s", options[i]);
        if (i == highlight) attroff(A_REVERSE);
    }

    // ✅ 타임아웃 정보
    mvprintw(HEIGHT - 3, 2, "선택하려면 Enter, 타임아웃 %d초...", time_left);

    // ✅ 깜빡임 최소화용 최종 출력
    wnoutrefresh(stdscr);
    doupdate();
}

int store_menu_ui() {
    setlocale(LC_ALL, "ko_KR.UTF-8");
    initscr();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);

    int highlight = 0;
    time_t start = time(NULL);
    int time_left = Time_Start;

    while (time_left > 0) {
        time_t now = time(NULL);
        time_left = Time_Start - (now - start);  

        draw_store_ui(highlight, time_left);

        int ch = getch();
        if (ch == KEY_UP) {
            highlight = (highlight + options_COUNT - 1) % options_COUNT;
        } else if (ch == KEY_DOWN) {
            highlight = (highlight + 1) % options_COUNT;
        } else if (ch == '\n') {
            endwin();
            return highlight;
        }
       napms(50);  // ✅ 깜빡임 줄이기, CPU 부하도 낮음
    }

    endwin();
    return options_COUNT-1;
}

