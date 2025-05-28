#include "Pve_game.h"
#include "Diver_ui.h"
#include "global.h"
#include "store.h"
#include "log.h"

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
        }
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
        }
    },
    {
        "쥐피티",
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
        }
    },
    {
        "닐론 머크스",
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
        }
    }
};

void monster_turn(Entity *monster, Entity *player, char *monster_action_result, int round, int turn, int selected_action, int monster_No) {
    // 몬스터별 행동 패턴 정의
    // 0: 공격, 1: 강화 공격, 2: 방어, 3: 충전, 4: 회피
    int pattern_didos[] = {3,0,3,2};      // 디도스      충전-공격-충전-방어
    int pattern_lootkit[]  = {3,0,2};        // 루키트       충전-공격-방어
    int pattern_gpt[]     = {3,0,3,1};      // 지피티티        충전-공격-충전-강화공격
    int pattern_boss[]   = {3,1,2,3,4,0,1}; // 보스           충전-강화공격-방어-충전-회피-공격-강화공격

    int *pattern;
    int pattern_length;

    if(round % 7 == 0){
        pattern = pattern_boss;
        pattern_length = sizeof(pattern_boss) / sizeof(int);
    } else {/*
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
        }*/
        switch(monster_No){
        case 0:
            pattern = pattern_didos;
            pattern_length = sizeof(pattern_didos) / sizeof(int);
            break;
        case 1:
            pattern = pattern_lootkit;
            pattern_length = sizeof(pattern_lootkit) / sizeof(int);
            break;
        case 2:
            pattern = pattern_gpt;
            pattern_length = sizeof(pattern_gpt) / sizeof(int);
            break;
        }
    
    }

    int action_index = pattern[turn % pattern_length];

    // 행동 실행
    switch (action_index) {
        case 0: // 공격
            if (monster->bit >= 1) {
                if (selected_action == 2) { // 플레이어 방어일때
                    player->data -= (monster->attack - player->defense);
                } else if(selected_action != 4) { // 플레이어 회피아닐때
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
                if (selected_action == 2) { // 플레이어 방어일때
                    player->data -= (monster->strong_attack - player->defense);
                } else if(selected_action != 4) { // 플레이어 회피아닐때
                    player->data -= monster->strong_attack;
                }
                monster->bit -= 5;
                snprintf(monster_action_result, 100, "강화 공격!");
            } else {
                snprintf(monster_action_result, 100, "BIT 부족!");
            }
            break;
        case 2: // 방어
            if(selected_action == 0 || selected_action == 1){
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
            if(selected_action == 0){ 
                monster->data += player->attack;
            } else if(selected_action == 1){
                monster->data += player->strong_attack;
            }
            monster->bit--;
            snprintf(monster_action_result, 100, "회피!");
            break;
        default:
            snprintf(monster_action_result, 100, "이상 행동");
    }
}

int wait_for_input_with_timeout(int *selected_action, int timeout_sec, int turn, int round, Entity player, Entity monster, char *player_action_result, char *monster_action_result,int monster_No) {
    time_t start_time = time(NULL);
    int ch;
    int action_confirmed = 0;

    while (1) {
        int elapsed = (int)(time(NULL) - start_time);
        int remaining = timeout_sec - elapsed;
        if (remaining <= 0) {
            if(action_confirmed == 1){
                return 1;
            } else {
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

        attron(COLOR_PAIR(2));
        box(stdscr, 0, 0);
        attroff(COLOR_PAIR(2));
        
        print_status(turn, remaining, round, player, monster, *selected_action);
        draw_ui(player, monster, round,monster_No);
        draw_game_time();//########################################

        mvprintw(30, 10, "플레이어 액션: %s", player_action_result);
        mvprintw(31, 10, "몬스터   액션: %s", monster_action_result);
        refresh();
    }
}

void bonus_round(int boss_count, Entity *player) {
    clear();

    // 입력모드 초기화
    nocbreak();
    noraw();
    cbreak();
    echo();
    curs_set(1);

    // 문제와 정답 배열
    char *questions[] = {
        "사용자 프로그램이 운영체제에 도움을 요청할 때 사용하는 함수를 뭐라고 하나요?",
        "하나의 컴퓨터를 여러 사용자가 동시에 사용하는 것처럼 보이게 하는 기술은?",
        "파일 디스크립터를 조작하는데 사용하는 시스템 호출 함수는?",
        "create()함수를 open()으로 대체하려고 할때 들어가야 할 플래그는?"
    };

    char *answers[] = {
        "시스템콜",
        "time sharing",
        "fcntl",
        "O_CREAT|O_WRONLY|O_TRUNC"
    };

    int total_questions = sizeof(questions) / sizeof(questions[0]);
    int idx = boss_count % total_questions;

    char input[50] = {0};

    int row, col;
    getmaxyx(stdscr, row, col);

   const char* monster_ascii[] = {
"⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀",
"⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣀⣄⣀⢀⢤⢤⡤⡤⣄⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀",
"⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡤⣞⣯⢿⣺⣗⣯⣫⣗⡯⣟⣗⣯⢯⢦⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀",
"⠀⠀⠀⠀⠀⠀⠀⠀⡠⡮⡯⣗⣿⢽⣟⣞⣗⢷⣳⢯⡷⡯⣞⡿⡽⣞⣦⠀⠀⠀⠀⠀⠀⠀⠀",
"⠀⠀⠀⠀⠀⠀⠀⣲⡳⣝⢽⢵⣻⢝⠎⡂⡪⡙⡺⣝⢽⣽⣫⢯⣟⣗⣯⢇⠀⠀⠀⠀⠀⠀⠀",
"⠀⠀⠀⠀⠀⠀⠀⣗⣽⣺⡽⣯⢳⢑⢁⢂⢐⠨⠐⠜⢜⢺⢺⢯⡷⣯⣯⢿⡅⠀⠀⠀⠀⠀⠀",
"⠀⠀⠀⠀⠀⠀⠀⣷⣻⣞⣟⢎⢕⢄⢢⠠⠐⠀⠅⠡⡡⡊⡌⡎⣿⣳⣯⢿⡕⠀⠀⠀⠀⠀⠀",
"⠀⠀⠀⠀⠀⠀⠀⠓⣗⡗⠡⡑⠄⡂⠄⡊⢌⠐⠨⡨⠐⠄⠌⠔⡁⡓⣯⡟⠀⠀⠀⠀⠀⠀⠀",
"⠀⠀⠀⠀⠀⠀⠀⠀⡜⡌⠨⠐⠍⡊⡫⠘⡄⠅⢕⡘⡘⡫⢙⠐⢌⢊⢞⠄⠀⠀⠀⠀⠀⠀⠀",
"⠀⠀⠀⠀⠀⠀⠀⠀⢊⠆⡈⠄⡁⡐⡀⠅⡊⡀⡂⢆⢐⠀⢂⢁⠂⡂⡇⠁⠀⠀⠀⠀⠀⠀⠀",
"⠀⠀⠀⠀⠀⠀⠀⠀⠀⠐⡀⢂⠠⠀⠄⢊⠤⢄⢢⢂⠅⡐⠀⠄⢂⠂⠅⠀⠀⠀⠀⠀⠀⠀⠀",
"⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠂⡂⢂⠂⠌⠄⢌⡐⣐⢐⠨⠀⠅⠌⡂⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀",
"⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠐⢐⢐⠨⠨⠩⡑⡅⡣⡹⢈⠊⠌⠢⡈⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀",
"⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡐⢔⠡⠡⡁⠢⢈⢐⠐⠄⢕⠡⣃⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀",
"⠀⠀⠀⠀⠀⠀⠀⠀⠀⡠⣪⠃⡂⠕⡑⢔⢡⢢⢢⢡⠱⠡⡑⠄⣿⣢⡀⠀⠀⠀⠀⠀⠀⠀⠀",
"⠀⠀⠀⠀⠀⠀⣀⢤⡺⣝⣽⠀⠀⠁⡊⠔⡐⠅⡂⠢⠡⢑⠀⠀⣳⣗⣿⣲⢦⣤⣀⢀⠀⠀⠀",
"⢀⣀⢤⡲⡼⣝⡮⣗⡯⣗⣯⠂⠀⠀⠀⠈⠄⠅⠌⠌⠈⠀⠀⠀⣗⣟⡾⣽⣻⢾⢽⣽⡽⣶⡤",
"⢸⣺⢽⣺⢯⣗⡯⣗⡯⣗⣟⡆⠀⢀⢤⣖⢾⢔⣖⡶⣤⢄⠀⠀⡷⣯⣟⡷⣯⣟⣿⣺⡽⣗⡯",
"⢸⢯⡯⣯⢟⡾⣽⣳⣻⢽⣺⡆⠄⠀⠀⠫⣯⣻⣪⡏⠁⠀⠁⠂⣿⣳⢷⣻⣗⣿⣺⣳⢿⢽⢯",
"⢸⡯⣯⡯⡿⣽⣳⣻⢞⣯⢷⢇⠀⠀⠀⢀⢹⢞⡾⡀⠀⠄⠀⢅⡿⣞⣯⣷⣻⣞⣷⣻⣽⣻⣗",
"⢸⡯⣷⣟⣯⢷⣻⣞⣯⢷⣻⢧⢀⠀⠀⠀⣾⡫⡯⣇⠀⠀⢀⢺⡽⣯⢷⣗⣿⣺⣞⣷⣻⣞⣮",
"⢸⣽⣳⣻⣞⣯⢷⣻⣺⢯⡯⣿⠀⠀⠀⢸⡵⡯⡯⣷⡀⠂⢐⣯⢿⣽⣻⣞⣷⣻⣞⣷⣻⣺⣳",
"⢸⣞⣷⣻⢾⡽⡯⣷⣻⣽⢽⡽⡎⠀⠀⣗⡯⣯⡻⣮⡣⠀⢸⣺⣟⣾⣳⣻⣞⣾⣳⢷⢯⡷⣗"
};

    int monster_ascii_lines = sizeof(monster_ascii) / sizeof(monster_ascii[0]);

    for (int i = 0; i < monster_ascii_lines; i++) {
    if (row / 2 - monster_ascii_lines / 2 + i >= 0 && row / 2 - monster_ascii_lines / 2 + i < row) {
        mvprintw(row / 2 - monster_ascii_lines / 2 + i, (col - strlen(monster_ascii[0])) / 2, "%s", monster_ascii[i]);
    }
}

    // 보너스 텍스트
    const char *bonus_text = "✨✨✨ *** 보너스 문제 *** ✨✨✨";
    attron(COLOR_PAIR(1) | A_BOLD);
    mvprintw(row / 2 - 2, (col - strlen(bonus_text)) / 2 + 7, "%s", bonus_text);
    attroff(COLOR_PAIR(1) | A_BOLD);

    // 문제 출력
    attron(COLOR_PAIR(2));
    mvprintw(row / 2, (col - strlen(questions[idx])) / 2 + 40, "%s", questions[idx]);
    attroff(COLOR_PAIR(2));

    // 입력 칸 표시 및 위치 설정
    int input_y = row / 2;
    int input_x = (col - 30) / 2;
    mvprintw(input_y+3, input_x, "입력: ");
    move(input_y+3, input_x + 7);

    refresh();

    timeout(-1);
    getnstr(input, 49);//이게 문제인가??

    // 입력 후 입력모드 원복
    noecho();
    curs_set(0);
/////////////////////////////////////////////////////////////////////스핑크스 지우기
    //erase();      // 메모리 상의 가상 화면을 모두 지움
    refresh();    // 지운 내용을 실제 터미널에 반영

/////////////////////////////////////////////////////////////////////스핑크스 지우기

    // 결과 출력
    if (strcmp(input, answers[idx]) == 0) {
        mvprintw(row / 2 + 6, (col - 20) / 2, "정답입니다! DATA + 50");
        player->data += 50;
        if (player->data > Player_DATA_BAR_WIDTH) Player_DATA_BAR_WIDTH=player->data;
    } 
    else if(strcmp(input, "I'm mhan") == 0){
        mvprintw(row / 2 +5, (col - 20) / 2,"💕💕💕💕💕💕I love mhan💕💕💕💕💕💕");       
        mvprintw(row / 2 + 6, (col - 20) / 2, "무조건!!! 정답입니다! DATA + 1000");
        player->data += 1000;
        if (player->data > Player_DATA_BAR_WIDTH) Player_DATA_BAR_WIDTH=player->data;
    }
    else {
        mvprintw(row / 2 + 6, (col - 20) / 2, "틀렸습니다...");
    }

    /*refresh();
    napms(10000);*/

     // 엔터 누르면 종료
    mvprintw(row / 2 + 8, (col - 20) / 2, "계속하려면 엔터를 누르세요...");
    refresh();
    while (getch() != '\n');  // 엔터 입력 대기
}