#include "Pve_game.h"
#include "Diver_ui.h"
#include "global.h"
#include "store.h"
#include "log.h"

extern int ROUND_MON_NO;
extern PlayerState Player;

void draw_data_bar(int y, int x, int data, int width) {
    mvprintw(y, x, "DATA : ");

    int filled_blocks = (int)((double)data / width * 10 + 0.5);  // 반올림

    for (int i = 0; i < 10; i++) {
        if (i < filled_blocks) addstr(FILLED_CHR);
        else addstr(" ");
    }
    printw(" %d/%d", data, width);
}

void draw_bit_bar(int y, int x, int bit, int width) {
    mvprintw(y+1, x-1, "⚡BIT : ");
    for (int i = 0; i < BIT_BAR_WIDTH; i++) {
        if (i < bit) addstr(FILLED_CHR);
        else addstr(" ");
    }
    printw(" %d/%d", bit, width);
}

void print_status(int turn, int remaining_time, int round, PlayerState Player, MonsterInfo monster, int selected_action) {
    mvprintw(1, 1, "라운드: %d", round);
    mvprintw(2, 1, "턴: %d | 남은 시간: %d초", turn, remaining_time);

    const char *actions[] = {
        "공격 (1 BIT)",
        "강화 공격 (5 BIT)",
        "방어 (0 Bit)",
        "BIT 충전",
        "회피 (1 BIT)"
    };

    for (int i = 0; i < 5; i++) {
        int action_x = TERM_WIDTH - 30;
        int action_y = TERM_HEIGHT - 12 + i;
        if (i == selected_action)
            attron(A_REVERSE);
        mvprintw(action_y + i, action_x, "%d. %s", i + 1, actions[i]);
        if (i == selected_action)
            attroff(A_REVERSE);
    }
}

void draw_ui(PlayerState Player, MonsterInfo monster, int round,int monster_No) {
    MonsterInfo current_monster;

    if(round % 7 == 0){
        current_monster = monsters[3];
    } else {
        current_monster = monsters[monster_No];//몬스터 랜덤하게 등장 시키기 ........
    }
    
    // 몬스터 UI출력
    for (int i = 0; i < 9; i++) {
        mvprintw(1 + i, 90, "%s", current_monster.art[i]);
    }
    mvprintw(11, 90, "👾 %s", current_monster.name);

    draw_data_bar(13, 90, current_monster.data, current_monster.max_data);
    
    draw_bit_bar(14, 90, current_monster.bit, BIT_BAR_WIDTH);
    mvprintw(17, 90, "공격력 : %d      방어력 : %d", current_monster.attack, current_monster.defense);
    
    // 플레이어 UI출력

    mvprintw(9, 5, "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⣀⣤⣠⣄⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀");
    mvprintw(10, 5, "⠀⠀⠀⠀⠀⠀⠀⠀⢠⣞⡷⣻⢞⡳⣏⢾⡹⣖⡄⠀⠀⠀⠀⠀⠀");
    mvprintw(11, 5, "⠀⠀⠀⠀⠀⠀⠀⠀⢾⣱⢏⡷⢫⠵⣩⠎⡵⠈⠜⠀⠀⠀⠀⠀⠀");
    mvprintw(12, 5, "⠀⠀⠀⠀⠀⠀⠀⠀⢻⣜⢯⡜⣣⢣⡑⢎⠐⣁⠦⠀⠀⠀⠀⠀⠀");
    mvprintw(13, 5, "⠀⠀⠀⠀⠀⠀⠀⠀⠘⣮⢳⡹⢰⡐⠄⠢⢄⢨⠁⡀⠀⠀⠀⠀⠀");
    mvprintw(14, 5,"⠀⠀⠀⠀⠀⠀⠀⠀⠀⣇⢃⠒⡠⠙⣂⠡⢌⢂⡑⠀⠀⠀⠀⠀⠀");
    mvprintw(15, 5, "⠀⠀⠀⠀⠀⠀⠀⢀⡸⢀⠣⢈⠰⡀⢼⠳⣌⠢⠀⠀⠀⠀⠀⠀⠀");
    mvprintw(16, 5, "⠀⠀⠀⠀⠀⢠⢢⠑⠂⡌⠄⠂⠑⠎⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀");
    mvprintw(17, 5, "⠀⠀⠀⠀⡠⠌⡀⠐⡀⢌⡑⠠⢈⠐⠠⠈⡐⠠⠠⠀⠀⠀⠀⠀⠀");
    mvprintw(18, 5, "⠀⢠⢣⡑⢆⠐⡀⠐⡆⢄⠣⠐⠨⠐⠠⠀⠐⠀⠂⠈⢄⠀⠀⠀⠀");

    mvprintw(21, 5, "🧑 플레이어");
    draw_data_bar(23, 5, Player.data, Player_DATA_BAR_WIDTH);
    draw_bit_bar(24, 5, Player.pve_start_bit, BIT_BAR_WIDTH);
    mvprintw(27, 5, "공격력 : %d      방어력 : %d", Player.atk_stat, Player.dfs_stat);
}

// 게임 초기화 함수
void initialize_game() {
    setlocale(LC_ALL, "");
    initscr();
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);    // 딜레이 시 (붉은 테두리)
    init_pair(2, COLOR_GREEN, COLOR_BLACK);  // 딜레이 시 (초록 테두리)
    
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(FALSE);
    
    attron(COLOR_PAIR(2));
    box(stdscr, 0, 0);
    attroff(COLOR_PAIR(2));
    refresh();
}


// 승리 화면 표시 함수
void display_victory_screen(int round, int cure_data) {
    erase();
    attron(COLOR_PAIR(2));
    box(stdscr, 0, 0);
    attroff(COLOR_PAIR(2));
    
    mvprintw(TERM_HEIGHT / 2 - 8, (TERM_WIDTH - 30) / 2, 
             "전투 후 부상 data 일부 획득 !!  +%d data", cure_data / 2);
    
    Player.data += (cure_data / 2);
    if (Player.data > Player_DATA_BAR_WIDTH) 
        Player_DATA_BAR_WIDTH = Player.data;
    
    if (round % 7 == 0) { // 보스 라운드
        mvprintw(TERM_HEIGHT / 2 - 7, (TERM_WIDTH - 30) / 2, 
                 "승리 보상 data 획득 !!         +100 data");
        mvprintw(TERM_HEIGHT / 2, (TERM_WIDTH - 30) / 2, "새로운 적이 등장했습니다!");
        Player.data += 100;
        if (Player.data > Player_DATA_BAR_WIDTH) 
            Player_DATA_BAR_WIDTH = Player.data;
    } else { // 일반 라운드
        mvprintw(TERM_HEIGHT / 2 - 7, (TERM_WIDTH - 30) / 2, 
                 "승리 보상 data 획득 !!         +30 data");
        if((round+1) % 7 !=0)
            mvprintw(TERM_HEIGHT / 2, (TERM_WIDTH - 30) / 2, "새로운 적이 등장했습니다!");
        else
            mvprintw(TERM_HEIGHT / 2, (TERM_WIDTH - 30) / 2, "🔥위험!! ** 보스 출현 **🔥");
        Player.data += 30;
        if (Player.data > Player_DATA_BAR_WIDTH) 
            Player_DATA_BAR_WIDTH = Player.data;
    }
    
    if (Player.data < Player_DATA_BAR_WIDTH) 
        Player_DATA_BAR_WIDTH = Player.data;
    
    mvprintw(TERM_HEIGHT / 2 + 4, (TERM_WIDTH - 30) / 2, 
             "계속하려면 Enter를 누르세요...");
    refresh();
    
    // Enter 키 대기
    int ch;
    while ((ch = getch()) != '\n' && ch != KEY_ENTER) {
        // 아무 동작 없음
    }
}

// 게임 종료 화면 표시 함수
void display_game_end() {
    erase();
    attron(COLOR_PAIR(2));
    box(stdscr, 0, 0);
    attroff(COLOR_PAIR(2));
    
    //int monster_no;


    if (Player.data <= 0)
        mvprintw(TERM_HEIGHT / 2, (TERM_WIDTH - 30) / 2, "Monster wins!");
    
    mvprintw(TERM_HEIGHT / 2 + 2, (TERM_WIDTH - 40) / 2, 
             "이제 연습은 끝났어..");
    mvprintw(TERM_HEIGHT / 2 + 3, (TERM_WIDTH - 40) / 2, 
             "절망의 끝에선 다다른 폰에게 모든 영광을.");
    refresh();
    while (getch() != '\n');
}
