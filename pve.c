//***플레이어 시작비트 늘리기,라운드마다 피해받은 data 절반 화복+보상 data
//이스틑 에그 한명균 교수님 기능 넣기
//보너스 문제 UI 꾸미기기
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <string.h>
#include <time.h>
#include <locale.h>
#include <wchar.h>
#include <ncursesw/ncurses.h>
#include <sys/time.h>
#include <poll.h>

#define DATA_BAR_WIDTH 10
#define BIT_BAR_WIDTH 10
#define TERM_HEIGHT 35
#define TERM_WIDTH 130

typedef struct {
    int x, y;
    int data;
    int bit;
    int attack;
    int strong_attack;
    int defense;
} Entity;


Entity player = {5, 8, 200, 5,20,100,20};//공경력 20, 방어력 20으로 올리고, 강화공격 100으로 올림 ++++++++++++++++++++++++++++. 초기 데이터 늘리고, 사각형 hp바를 모듈러 계산하여 처리하자. ++++
Entity monster = {50, 2, 10, 5,10,50,10};


typedef struct {
    char *name;
    char *art[5];
} MonsterInfo;

MonsterInfo monsters[] = {
    {
        "쥬니온 발록 v.6",
        {
            " /\\_/\\",
            "( o.o )",
            " > ^ <",
            "        ",
            "        "
        }
    },
    {
        "데스나이트 v.7",
        {
            "  |||||",
            " [ o_o ]",
            " /|/|/|\\",
            "  / \\  ",
            "        "
        }
    },
    {
        "AI 타이탄",
        {
            " /--\\_/--\\ ",
            "| 0   0 |",
            "|   ∆   |",
            " \\__^__/",
            "   ||   "
        }
    },
    {
        "Boss Junion",
        {
             "     /\\🔥  /\\     ", 
            "    /  \\__/  \\    ",  
            "  <=|  ⚪  ⚪  |=> ", 
            "     |   💠   |    ",
            "    /|__/⚙__|\\   "
        }
    }
}; 

void draw_data_bar(int y, int x, int data,int width) {
    mvprintw(y, x, "DATA : [");
    for (int i = 0; i < width; i++) {
        if (i < data) addstr("▰");
        else addstr("▱");
    }
    printw("] %d/%d", data, width);
}

void draw_bit_bar(int y, int x, int bit, int width) {
    mvprintw(y, x, "BIT:  [");
    for (int i = 0; i < BIT_BAR_WIDTH; i++) {
        if (i < bit) addstr("■");
        else addstr("□");
    }
    printw("] %d/%d", bit, width);
}

void print_status(int turn, int remaining_time, int round, Entity player, Entity monster, int selected_action) {//라운드,턴,5개의 선택명령어 출력 및 하이라이팅
    mvprintw(0, 0, "라운드: %d", round);
    mvprintw(1, 0, "턴: %d | 남은 시간: %d초", turn, remaining_time);

    const char *actions[] = {
        "공격 (1 BIT)",
        "강화 공격 (3 BIT)",
        "방어 (0 Bit)",
        "BIT 충전",
        "회피 (1 BIT)"
    };

    for (int i = 0; i < 5; i++) {
        int action_x = TERM_WIDTH - 30;
        int action_y = TERM_HEIGHT - 6 + i;
        if (i == selected_action)
            attron(A_REVERSE);
        mvprintw(action_y + i, action_x, "%d. %s", i + 1, actions[i]);
        if (i == selected_action)
            attroff(A_REVERSE);
    }
}

void draw_ui(Entity player, Entity monster, int round) { //몬스터와 플레이어의 머리부터 발끝까지 UI 출력
////////////////////////////////////////////////////////////
   /* erase(); // 화면 지우기

    // 박스 테두리 그리기
   attron(COLOR_PAIR(2));
    
    box(stdscr, 0, 0);
    
   attroff(COLOR_PAIR(2));*/

////////////////////////////////////////////////////////////////


    MonsterInfo current_monster;

    if(round % 7 == 0){
        current_monster=monsters[3];
    }else{
        current_monster = monsters[(round - 1) % 3];
    }
    //몬스터 UI출력
    for (int i = 0; i < 5; i++) {
        mvprintw(6 + i, 90, "%s", current_monster.art[i]);
    }
    mvprintw(11, 90, "%s", current_monster.name);
    draw_data_bar(12, 90, monster.data,DATA_BAR_WIDTH);
    draw_bit_bar(13, 90, monster.bit, BIT_BAR_WIDTH);
    mvprintw(15,90,"공격력 : %d      방어력 : %d",monster.attack,monster.defense);
    

    //플레이어 UI출력
    mvprintw(18, 5, "  (\\_/) ");
    mvprintw(19, 5, " ( •_•)");
    mvprintw(20, 5, "/ >🔥> ");
    mvprintw(22, 5, "플레이어 Lv.10");
    draw_data_bar(23, 5, player.data,DATA_BAR_WIDTH);
    draw_bit_bar(24, 5, player.bit, BIT_BAR_WIDTH);
    mvprintw(26,5,"공격력 : %d      방어력 : %d",player.attack,player.defense);

}
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@여기 부터 @@@@@@@@@@@@@@@@@@2
int action = 0;
void monster_turn(Entity *monster, Entity *player, char *monster_action_result, int round, int turn, int selected_action) {
    // 몬스터별 행동 패턴 정의
    // 0: 공격, 1: 강화 공격, 2: 방어, 3: 충전, 4: 회피
    int pattern_junion[] = {3,0,3,2};      // 쥬니온 발록      충전-공격-충전-방어
    int pattern_death[]  = {3,0,2};   // 데스나이트            충전-공격-방어
    int pattern_ai[]     = {3,0,3,1};   // AI 타이탄          충전-공격-충전-강화공격
    int pattern_boss[]={3,1,2,3,4,0,1}; //보스                    충전-강화공격-방어-충전-회피-공격-강화공격

    int *pattern;
    int pattern_length;

    if(round % 7 ==0){
        pattern = pattern_boss;
        pattern_length = sizeof(pattern_boss) / sizeof(int);
    }else{
        switch ((round - 1) % 3) {
        case 0:
            pattern = pattern_junion;
            pattern_length = sizeof(pattern_junion) / sizeof(int);
            break;
        case 1:
            pattern = pattern_death;
            pattern_length = sizeof(pattern_death) / sizeof(int);
            break;
        case 2:
            pattern = pattern_ai;
            pattern_length = sizeof(pattern_ai) / sizeof(int);
            break;
    }
    }
    

    int action_index = pattern[turn % pattern_length];

    // 행동 실행
    switch (action_index) {
        case 0: // 공격
            if (monster->bit >= 1) {
                if (selected_action==2) { //플레이어 방어일때
                    player->data -= (monster->attack - player->defense);
                }else if(selected_action!=4) { //플레이어 회피아닐때
                    player->data -= monster->attack;
                }
                monster->bit--;
                snprintf(monster_action_result, 100, "공격!");
            } else {
                snprintf(monster_action_result, 100, "BIT 부족!");
            }
            break;
        case 1: // 강화 공격
            if (monster->bit >= 5) {
                if (selected_action==2) { //플레이어 방어일때
                    player->data -= (monster->strong_attack - player->defense);
                }else if(selected_action!=4) { //플레이어 회피아닐때
                    player->data -= monster->strong_attack;
                }
                monster->bit-=5;
                snprintf(monster_action_result, 100, "강화 공격!");
            } else {
                snprintf(monster_action_result, 100, "BIT 부족!");
            }
            break;
        case 2: // 방어
            if(selected_action==0 || selected_action==1){
                monster->data += monster->defense;
            }
            snprintf(monster_action_result, 100, "방어!");
            break;
        case 3: // BIT 충전
            if (monster->bit < BIT_BAR_WIDTH) {
                monster->bit++;
                snprintf(monster_action_result, 100, "BIT 충전!");
            } else {
                snprintf(monster_action_result, 100, "BIT가 꽉 찼습니다!");
            }
            break;
        case 4: // 회피
            if(selected_action==0){ 
                monster->data += player->attack;
            }else if(selected_action==1){
                monster->data+=player->strong_attack;
            }
            monster->bit--;
            snprintf(monster_action_result, 100, "회피!");
            break;
        default:
            snprintf(monster_action_result, 100, "이상 행동");
    }
}

int wait_for_input_with_timeout(int *selected_action, int timeout_sec, int turn,int round, Entity player, Entity monster, char *player_action_result, char *monster_action_result) {
    time_t start_time = time(NULL);
    int ch;
    int action_confirmed = 0;

    while (1) {
        int elapsed = (int)(time(NULL) - start_time);
        int remaining = timeout_sec - elapsed;
        if (remaining <= 0) {
            if(action_confirmed == 1){
                return 1;
            }else{
                return 0;
            }
        }
        timeout(100);
        ch = getch();
        if (ch != ERR) {
            if (!action_confirmed) {
                switch (ch) {
                    case KEY_UP:
                        *selected_action = (*selected_action + 4) % 5;
                        break;
                    case KEY_DOWN:
                        *selected_action = (*selected_action + 1) % 5;
                        break;
                    case '\n':
                        action_confirmed = 1;
                        break;
                    case 'q':
                        endwin();
                        exit(0);
                }
            } else {
                if (ch == 'q') {
                    endwin();
                    exit(0);
                }
            }
        }

        erase();
        print_status(turn, remaining, round, player, monster, *selected_action);
        draw_ui(player, monster, round);
        mvprintw(33, 10, "플레이어 액션: %s", player_action_result);
        mvprintw(34, 10, "몬스터   액션: %s", monster_action_result);
        refresh();
    }
}


void bonus_round(int boss_count,Entity *player) {
    clear();

    // 🛠️ 입력모드 초기화
    // 일반적으로 프로그램 시작 시 한번만 하면 되지만, 여기서 다시 설정 보장
    nocbreak();          // 혹시라도 raw 모드였으면 해제
    noraw();
    cbreak();            // 라인 버퍼 사용, Enter 치기 전까지 입력 허용
    echo();              // 입력한 문자 보이기
    curs_set(1);         // 커서 보이기

    // 문제와 정답 배열
    char *questions[] = {
        "사용자 프로그램이 운영체제에 도움을 요청할 때 사용하는 함수를 뭐라고 하나요?",
        "하나의 컴퓨터를 여러 사용자가 동시에 사용하는 것처럼 보이게 하는 운영체제 기술을 뭐라고 하나요?",
        "파일 디스크립터를 조작하는데 사용하는 시스템 호출 함수는 뭔가요?"
    };

    char *answers[] = {
        "시스템콜",
        "time sharing",
        "fcntl"
    };

    int total_questions = sizeof(questions) / sizeof(questions[0]);
    int idx = boss_count % total_questions;

    char input[50] = {0};

    int row, col;
    getmaxyx(stdscr, row, col);

    const char *sphinx_face[] = {
    "                             _________",
    "                          .~         ~.",
    "                        .'  ()     ()  '.",
    "                       /      ^ ^       \\",
    "                      |     _______      |",
    "                      |    /_______\\     |",
    "                      |    \\__   __/     |",
    "                     /|      | | |       |\\",
    "                    /_|______|_|_|_______|_\\",
    "                  .'   /     | | |     \\   '.",
    "                .'    /      | | |      \\    '.",
    "               /_____/       |_|_|       \\_____\\",
    "              |     |        |   |        |     |",
    "              |_____|________|___|________|_____|"
};
int sphinx_lines = sizeof(sphinx_face) / sizeof(sphinx_face[0]);

for (int i = 0; i < sphinx_lines; i++) {
    mvprintw(row / 2 - sphinx_lines - 3 + i, (col - strlen(sphinx_face[i])) / 2-22, "%s", sphinx_face[i]);
}



    // 보너스 텍스트////////////////////////////////////////////////////////////////////////
    const char *bonus_text = "✨✨✨ *** 보너스 문제 *** ✨✨✨";
    attron(COLOR_PAIR(1) | A_BOLD);
    mvprintw(row / 2 - 2, (col - strlen(bonus_text)) / 2 -6, "%s", bonus_text);
    attroff(COLOR_PAIR(1) | A_BOLD);

    // 문제 출력
    attron(COLOR_PAIR(2));
    mvprintw(row / 2, (col - strlen(questions[idx])) / 2, "%s", questions[idx]);
    attroff(COLOR_PAIR(2));


    ////////////////////////////////////////////////////////////////////////////

    // 문제 출력
    //mvprintw(row / 2 - 3, (col - strlen(questions[idx])) / 2, "***보너스 문제***");
    //mvprintw(row / 2 - 2, (col - strlen(questions[idx])) / 2, "%s", questions[idx]);

    
    // 입력 칸 표시 및 위치 설정
    int input_y = row / 2;
    int input_x = (col - 30) / 2;
    mvprintw(input_y+3, input_x, "입력: ");
    move(input_y+3, input_x + 7); // 입력 위치 정확히 지정

    refresh(); // 화면 갱신 후 입력 대기

    // ⛔️ getnstr가 nodelay 모드에서 바로 리턴하는 경우 방지
    timeout(-1); // 입력 기다림 (blocking 모드)

    getnstr(input, 49); // 문자열 입력 받기

    // 입력 후 입력모드 원복
    noecho();
    curs_set(0);

    // 결과 출력
    if (strcmp(input, answers[idx]) == 0) {
        mvprintw(row / 2 + 5, (col - 20) / 2, "정답입니다! HP +3");
        player->data += 3;
        if (player->data > DATA_BAR_WIDTH) player->data = DATA_BAR_WIDTH;
    } else {
        mvprintw(row / 2 + 5, (col - 20) / 2, "틀렸습니다...");
    }

    refresh();
    napms(10000); // 10초 대기
}


int main() {
////////////////////////////////////////////////////////////////////////////////////////
    //start_color();
    //init_pair(1, COLOR_RED, COLOR_BLACK);    // 딜레이 시 (붉은 테두리)
    //init_pair(2, COLOR_GREEN, COLOR_BLACK);  // 일반 시 (초록 테두리)
////////////////////////////////////////////////////////////////////////////////////////
    setlocale(LC_ALL, "");
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(FALSE);
    srand(time(NULL));

    int turn = 0;
    int round = 1;
    int selected_action = 0;
    char player_action_result[100] = "";
    char monster_action_result[100] = "";

    int boss_count=0;

    while (player.data > 0 && monster.data > 0) {
        print_status(turn, 2, round, player, monster, selected_action);
        draw_ui(player, monster, round);
        mvprintw(33, 10, "플레이어 액션: %s", player_action_result);
        mvprintw(34, 10, "몬스터   액션: %s", monster_action_result);
        refresh();

        int acted = wait_for_input_with_timeout(&selected_action, 3, turn, round, player, monster, player_action_result, monster_action_result);
        if (!acted) selected_action = 3;

        switch (selected_action) {
            case 0:
                if (player.bit >= 1) {
                    monster.data -= player.attack;
                    player.bit--;
                    snprintf(player_action_result, 100, "공격!");
                } else snprintf(player_action_result, 100, "BIT 부족!");
                break;
            case 1:
                if (player.bit >= 5) {
                    monster.data -= player.strong_attack;
                    player.bit -= 5;
                    snprintf(player_action_result, 100, "강화 공격!");
                } else snprintf(player_action_result, 100, "BIT 부족!");
                break;
            case 2:
                snprintf(player_action_result, 100, "방어!");
                break;
            case 3:
                if (player.bit < BIT_BAR_WIDTH) {
                    player.bit++;
                    snprintf(player_action_result, 100, "BIT 충전!");
                } else snprintf(player_action_result, 100, "BIT가 꽉 찼습니다!");
                break;
            case 4:
                if (player.bit >= 1) {
                    player.bit--;
                    snprintf(player_action_result, 100, "회피!");
                } else snprintf(player_action_result, 100, "BIT 부족!");
                break;
        }

        monster_turn(&monster, &player, monster_action_result, round, turn,selected_action); //selected_action은 플레이어 선택을 의미.

        turn++;

        if (monster.data <= 0) {

            //turn=0;?????
             if (round % 7 == 0) { // 보스 라운드라면
                bonus_round(boss_count,&player);
                boss_count++;   
            }

            round++;
            monster.data = DATA_BAR_WIDTH;
            monster.bit = 5;
            turn = 0;

            erase();
            if(round % 7 == 0){
                mvprintw(TERM_HEIGHT / 2, (TERM_WIDTH - 30) / 2, "위험!! ** 보스 출현 **");
            }else{
                mvprintw(TERM_HEIGHT / 2, (TERM_WIDTH - 30) / 2, "새로운 적이 등장했습니다!");
            }
            refresh();
            napms(1500);
        }

    
    }

    erase();
    if (player.data <= 0 && monster.data <= 0)
        mvprintw(TERM_HEIGHT / 2, (TERM_WIDTH - 30) / 2, "무승부!");
    else if (player.data <= 0)
        mvprintw(TERM_HEIGHT / 2, (TERM_WIDTH - 30) / 2, "Monster wins!");
    else if (monster.data <= 0)
        mvprintw(TERM_HEIGHT / 2, (TERM_WIDTH - 30) / 2, "Player wins!");

    mvprintw(TERM_HEIGHT / 2 + 2, (TERM_WIDTH - 40) / 2, "Press ENTER to exit the game...");
    refresh();
    while (getch() != '\n');

    endwin();
    return 0;
}
