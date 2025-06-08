#include <ncurses.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <locale.h> 
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
const char *Special_ablity[Special_ablity_COUNT+1] = {"시작  Bit", "Data 수급량","강한 공격 계수",
                                                        "차지  속도","카운터 공격 계수", "강한차지 충전량","모든 아이템"};
const char *Special_ablity_coefficient[Special_ablity_COUNT+1] = { "  +1", "x 1.5배", "x 1.2배",
                                                        " -0.5s","x 1.5배", "x 1.2배","구매 완료"};

extern int Player_DATA_BAR_WIDTH;

void call_store(int time_limit){ 
    clear();
    refresh();
    int choice =1;
    Player.store_access = 0;
    time_t start = time(NULL);
    rand_ability_no_dup();
    while(choice!=3){
        choice = store_menu_ui(time_limit,start); // 상점 기능 호출
        initscr();
        handle_buy(choice);
    }
    Player.store_access = 1;
}

void handle_buy(int choice) {
    if (choice == 0) {
        if(Player.data>10*Player.buy_atk_cnt){
            Player_DATA_BAR_WIDTH-=10*Player.buy_atk_cnt;
           Player.data-=10*Player.buy_atk_cnt++;
            Player.atk_stat +=10;
            write_log_file("공격력  강화","upgrade_log.txt");}
        else{write_log_file("구매가 불가능합니다","upgrade_log.txt");}
    } 
    else if (choice == 1) {
        if(Player.data>10*Player.buy_dfs_cnt){
            Player_DATA_BAR_WIDTH-=10*Player.buy_dfs_cnt;
            Player.data-=10*Player.buy_dfs_cnt++;
            Player.dfs_stat +=10;
            write_log_file("방어력  강화","upgrade_log.txt");}
        else{write_log_file("구매가 불가능합니다","upgrade_log.txt");}
    }
    else if (choice == 2) {
        if(Player.data>50&&Player.ability_sort!=Special_ablity_COUNT){
            Player_DATA_BAR_WIDTH-=50;
            Player.data-=50;
            Player.ability_dup_check[Player.ability_sort] = true;
            ability_upgrade(Player.ability_sort);
            write_log_file("특수능력 구매","upgrade_log.txt");
            rand_ability_no_dup();
            mvprintw(9, 42, "|                    ");
            mvprintw(10, 42,"|                    ");
        }
        else{write_log_file("구매가 불가능합니다","upgrade_log.txt");}
    }    
    else if(choice ==3){
        mvprintw(LINES - 2, 1, "상점에서 나갔습니다.");
    }
    refresh();
}

void rand_ability_no_dup(){
    int cnt =0;
    while(1){
        if(cnt>Special_ablity_COUNT-1){Player.ability_sort = Special_ablity_COUNT; return;}
        if(!Player.ability_dup_check[cnt++]){break;}//false, 즉 비 구매 상품이 존재할때
    }

    do{
    Player.ability_sort = rand()%Special_ablity_COUNT;
    }while(Player.ability_dup_check[Player.ability_sort]);
}

void ability_upgrade(int ability_sort){
    
         if(ability_sort == 0){Player.pve_start_bit +=1;}
    else if(ability_sort == 1){Player.pve_data_intake=(Player.pve_data_intake*3)/2;}
    else if(ability_sort == 2){Player.pve_strong_atk_stat =6;}
    else if(ability_sort == 3){Player.pvp_charge_minus = 500;}
    else if(ability_sort == 4){Player.pvp_counter_atk_power_stat = 1.5;}
    else if(ability_sort == 5){Player.pvp_charge_strong = 1.2;}

}

void draw_store_ui(int highlight, int time_left) {
    setlocale(LC_ALL, "ko_KR.utf-8");
    if(!cnt)erase();
    if(cnt<1)cnt++;
    draw_border(3);  // erase() 제거로 화면 전체 초기화 방지
    draw_game_time();
    
    //  1사분면 (플레이어 정보 + item 정보보) 
    mvprintw(2, WIDTH / 2 + 20, "[%s 정보]",Player.nick);
    mvprintw(4, WIDTH / 2 + 20, "데이터: %4d", Player.data);             if(Player.ability_dup_check[0]==true){mvprintw(4, WIDTH / 2 + 35, "PVE 시작 비트: %d", Player.pve_start_bit);}
    mvprintw(5, WIDTH / 2 + 20, "공격력: %4d", Player.atk_stat);         if(Player.ability_dup_check[1]==true){mvprintw(5, WIDTH / 2 + 35, "PVE Data 수급량: %d", Player.pve_data_intake);}
    mvprintw(6, WIDTH / 2 + 20, "방어력: %4d", Player.dfs_stat);         if(Player.ability_dup_check[2]==true){mvprintw(6, WIDTH / 2 + 35, "PVE 강력 공격 계수: %d", Player.pve_strong_atk_stat);}
                                                                        if(Player.ability_dup_check[3]==true){mvprintw(7, WIDTH / 2 + 35, "PVP 차지 속도: -%dms", Player.pvp_charge_minus);}
                                                                        if(Player.ability_dup_check[4]==true){ mvprintw(8, WIDTH / 2 + 35, "PVP 카운터 공격계수: %1.1f", Player.pvp_counter_atk_power_stat);}
                                                                        if(Player.ability_dup_check[5]==true){ mvprintw(9, WIDTH / 2 + 35, "PVP 강한차지 충전량 : %1.1f", Player.pvp_charge_strong);}


    //  2사분면 (상점 상품)
    mvprintw(2, 2, "[상점 상품]");
     
    mvprintw(4, 2,  "------------------------------------------------------------------------------");
    mvprintw(5, 2,  "|                        |                        |                          |");
    mvprintw(6, 2,  "|     공격력  강화       |     방어력  강화       |       특수능력 구매      |");
    mvprintw(7, 2,  "|          +%3d강        |         +%3d강         |          %s           |",Player.buy_atk_cnt-1,Player.buy_dfs_cnt-1, Player.ability_sort==Special_ablity_COUNT?"     ":Player.ability_sort<PVE_ablity_COUNT?"[PVE]":"[PVP]");
    mvprintw(8, 2,  "|                        |                        |                          |");
    mvprintw(9, 2, "|                        |                        |       %s       ",Special_ablity[Player.ability_sort]); 
    mvprintw(9, 79,"|");
    mvprintw(10, 2, "|                        |                        |           %s       ",Special_ablity_coefficient[Player.ability_sort]);
    mvprintw(10, 79,"|");
    mvprintw(11, 2, "|                        |                        |                          |");
    mvprintw(12, 2, "|    필요 데이터: %3d    |    필요 데이터: %3d    |      %s %s     |", 10 * Player.buy_atk_cnt, 10 * Player.buy_dfs_cnt,Player.ability_sort==Special_ablity_COUNT?"            ":"필요 데이터:",Player.ability_sort==Special_ablity_COUNT?"  ":"50");
    mvprintw(13, 2, "------------------------------------------------------------------------------");

    // 3사분면 (로그)
    int log_y_offset = HEIGHT / 2;
    mvprintw(log_y_offset, 2, "[업그레이드 내역]");
    read_log_file("upgrade_log.txt", log_y_offset,2);


    // 4사분면 (선택 메뉴)
    int select_y_offset = HEIGHT / 2;
    int select_x_offset = WIDTH / 2  + 23;

    mvprintw(select_y_offset, select_x_offset, "[상점 선택]");
    for (int i = 0; i < options_COUNT; ++i) {
        if (i == highlight) attron(A_REVERSE);
        mvprintw(select_y_offset + 2 + i, select_x_offset, "%s", options[i]);
        if (i == highlight) attroff(A_REVERSE);
    }

    // 타임아웃 정보
    mvprintw(HEIGHT - 3, 2, "선택하려면 Enter, 타임아웃 %d초...", time_left);

    wnoutrefresh(stdscr);
    doupdate();
}

int store_menu_ui(int time_limit,time_t start) {
    setlocale(LC_ALL, "ko_KR.UTF-8");
    initscr();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);

    int highlight = 0;
    int time_left = time_limit;

    while (time_left > 0) {
        time_t now = time(NULL);
        time_left = time_limit - (now - start);  

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
        napms(50);  
    }

    endwin();
    return options_COUNT - 1;
}

