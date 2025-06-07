#include "Pve_game.h"
#include "Diver_ui.h"
#include "global.h"
#include "store.h"

extern int ROUND_MON_NO;
extern PlayerState Player;


MonsterInfo monsters[] = {
    {
        "디도스",
        {
            "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀",
            "⠀⠀⠀⠀⠀⠀⡀⢄⠠⡐⢄⢀⠠⡠⡰⠢⠀⠀",
            "⠀⠀⠀⢀⢊⢮⢮⡳⣝⣮⠮⡦⡣⣕⢥⠀⠀⠀",
            "⠀⠀⡀⡃⢧⢯⣷⣻⡽⠎⠀⣹⣻⡜⣢⠀⠀⠀",
            "⠀⠀⢳⡀⢈⢳⣗⡯⠣⠀⠀⣺⣺⣽⡪⠀⠀⠀",
            "⠀⠀⠘⢶⣀⡴⣯⢿⡴⡤⢞⢽⣽⣳⢵⠂⠀⠀",
            "⠀⠀⠀⠚⠮⡯⡣⡣⡣⣱⣵⡿⣝⣮⠁⠀⠀⠀",
            "⠀⠀⠀⠀⠀⠙⠹⡳⡻⠽⡚⠊⠁⠁⠀⠀⠀⠀",
            "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀"
        },
        150, 150, 3, 20,100,20  // max_data, data, bit, attack, strong_attack, defense
    },
    {
        "루트킷",
        {
            "⠀⠀⠀⠀⠀⡠⢢⢣⢱⢑⢕⢢⢂⠀⠀⠀⠀⠀",
            "⠀⠀⠀⠀⡌⡎⢎⢆⢇⢣⢃⢇⢣⠣⠀⠀⠀⠀",
            "⠂⠀⠐⠀⢕⢢⠀⡀⡕⢕⡁⢀⢐⠭⠀⠀⠀⠂",
            "⠀⠀⠀⠀⡕⡕⢕⢱⢑⢕⢜⢜⢜⢜⠀⠀⢀⠀",
            "⠀⠂⠁⡐⡕⡜⡜⢜⢜⢸⢨⢢⢣⠪⣂⠀⠀⠀",
            "⠄⠀⡔⡜⡌⡆⡇⢇⢣⢱⢑⢅⢇⢣⠣⡅⠄⠀",
            "⠀⠀⠊⠊⠈⠀⠈⢊⢎⢎⠪⠀⠀⠈⠘⠘⠀⠀",
            "⠠⠀⠀⠀⠀⠀⠀⠀⠱⠑⠁⠀⠀⠂⠀⠀⠀⡀",
            ""
        },
        150, 150, 3, 25, 125, 25   // max_data, data, bit, attack, strong_attack, defense
    },
    {
        "G피티",
        {
            "⠀⠀⠀⠀⢀⣴⠾⠟⠻⠷⣦⣀⣀⣀⠀⠀⠀⠀",
            "⠀⠀⣀⣠⣿⠁⠀⣀⣤⡾⠟⠋⠉⠙⠻⣦⡀⠀",
            "⢀⣾⠛⢹⡇⠀⣾⠋⠁⣀⡤⠶⣦⣄⡀⠸⣷⠀",
            "⣾⡇⠀⢸⡇⠀⣿⡴⠛⠛⢦⣄⡀⠙⠻⢷⣟⠀",
            "⢹⣇⠀⠘⠷⣄⣿⠀⠀⠀⠀⣿⠙⢲⡄⠀⢹⣇",
            "⠀⣽⢷⣦⣄⠈⠙⠳⣤⣤⠞⣷⠀⢸⡇⠀⢸⡿",
            "⠀⢿⡆⠈⠙⠻⠶⠚⠉⢀⣠⡿⠀⣸⣇⣤⡿⠁",
            "⠀⠈⠻⣦⣄⣀⣠⣴⡾⠛⠉⠀⢀⣾⠋⠁⠀⠀",
            "⠀⠀⠀⠀⠉⠉⠉⠻⢶⣦⣴⡶⠟⠁⠀⠀⠀⠀"
        },
        200, 200, 3, 20, 100, 40  // max_data, data, bit, attack, strong_attack, defense
    },
    {
        "[BOSS] Cyber MUSK",
        {
            "⠀⠀⠀⢐⢵⡺⠪⠩⠋⠟⠚⠪⢓⡽⢖⠀⠀⠀",
            "⠀⠀⠀⢉⢗⠜⠈⢀⠀⠀⠀⢐⠨⡸⡊⠄⠀⠀",
            "⠀⠀⠀⡠⠱⠡⠂⠀⠀⠀⠀⠐⠔⡈⠢⠁⠀⠀",
            "⠀⠀⠀⢨⠠⣏⠪⠳⡄⠰⠪⢍⠣⡀⠍⠀⠀⠀",
            "⠀⠀⠀⠈⢢⠀⠁⠁⠄⢀⠁⠀⠂⢤⠃⠀⠀⠀",
            "⠀⠀⠀⠀⠸⡹⠀⠀⠾⡔⠂⢀⠱⢀⠀⠀⠀⠀",
            "⠀⠀⠀⡠⣸⣺⣀⢓⣚⣒⡒⠢⠨⣺⡥⣀⠀⠀",
            "⣦⣿⣽⣿⣽⣷⢵⣢⠢⡂⣢⣵⣱⣿⢎⢼⣿⡦",
            "⣿⣿⣿⣷⣿⣿⣿⣾⠢⢅⢫⣿⣿⣿⣧⢕⣿⣿"
        },
        300, 300, 5, 45, 225, 45  // max_data, data, bit, attack, strong_attack, defense (보스)
    }
};


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
        "강화 공격 (4 BIT)",
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

    mvprintw(21, 5, "🧑 %s",Player.nick);
    draw_data_bar(23, 5, Player.data, Player_DATA_BAR_WIDTH);
    draw_bit_bar(24, 5, Player.bit, BIT_BAR_WIDTH);
    mvprintw(27, 5, "공격력 : %d      방어력 : %d", Player.atk_stat, Player.dfs_stat);
}

// 게임 초기화 함수
void initialize_game() {
    setlocale(LC_ALL, "");
    initscr();
    
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(FALSE);
    
    draw_border(1);
    refresh();
}


// 승리 화면 표시 함수
void display_victory_screen(int round, int cure_data) {
    erase();
    draw_border(1);
    
    mvprintw(TERM_HEIGHT / 2 - 8, (TERM_WIDTH - 30) / 2, 
             "전투 후 부상 data 일부 획득 !!  +%d data", (unsigned)cure_data / 2);
    
    Player.data += (cure_data / 2);
    if (Player.data > Player_DATA_BAR_WIDTH) 
        Player_DATA_BAR_WIDTH = Player.data;
    
    if (round % 7 == 0) { // 보스 라운드
        mvprintw(TERM_HEIGHT / 2 - 7, (TERM_WIDTH - 30) / 2, 
                 "승리 보상 data 획득 !!         +%d data",3*Player.pve_data_intake);
        mvprintw(TERM_HEIGHT / 2, (TERM_WIDTH - 30) / 2, "새로운 적이 등장했습니다!");
        Player.data += 3*Player.pve_data_intake;
        if (Player.data > Player_DATA_BAR_WIDTH) 
            Player_DATA_BAR_WIDTH = Player.data;
    } else { // 일반 라운드
        mvprintw(TERM_HEIGHT / 2 - 7, (TERM_WIDTH - 30) / 2, 
                 "승리 보상 data 획득 !!         +%d data",Player.pve_data_intake);
        if((round+1) % 7 !=0)
            mvprintw(TERM_HEIGHT / 2, (TERM_WIDTH - 30) / 2, "새로운 적이 등장했습니다!");
        else
            mvprintw(TERM_HEIGHT / 2, (TERM_WIDTH - 30) / 2, "🔥위험!! ** 보스 출현 **🔥");
        Player.data += Player.pve_data_intake;
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

