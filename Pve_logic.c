#include "Pve_game.h"

void monster_turn(Entity *monster, Entity *player, char *monster_action_result, int round, int turn, int selected_action) {
    // 몬스터별 행동 패턴 정의
    // 0: 공격, 1: 강화 공격, 2: 방어, 3: 충전, 4: 회피
    int pattern_junion[] = {3,0,3,2};      // 쥬니온 발록      충전-공격-충전-방어
    int pattern_death[]  = {3,0,2};        // 데스나이트       충전-공격-방어
    int pattern_ai[]     = {3,0,3,1};      // AI 타이탄        충전-공격-충전-강화공격
    int pattern_boss[]   = {3,1,2,3,4,0,1}; // 보스           충전-강화공격-방어-충전-회피-공격-강화공격

    int *pattern;
    int pattern_length;

    if(round % 7 == 0){
        pattern = pattern_boss;
        pattern_length = sizeof(pattern_boss) / sizeof(int);
    } else {
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

int wait_for_input_with_timeout(int *selected_action, int timeout_sec, int turn, int round, Entity player, Entity monster, char *player_action_result, char *monster_action_result) {
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
        draw_ui(player, monster, round);
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

    // 보너스 텍스트
    const char *bonus_text = "✨✨✨ *** 보너스 문제 *** ✨✨✨";
    attron(COLOR_PAIR(1) | A_BOLD);
    mvprintw(row / 2 - 2, (col - strlen(bonus_text)) / 2 -6, "%s", bonus_text);
    attroff(COLOR_PAIR(1) | A_BOLD);

    // 문제 출력
    attron(COLOR_PAIR(2));
    mvprintw(row / 2, (col - strlen(questions[idx])) / 2, "%s", questions[idx]);
    attroff(COLOR_PAIR(2));

    // 입력 칸 표시 및 위치 설정
    int input_y = row / 2;
    int input_x = (col - 30) / 2;
    mvprintw(input_y+3, input_x, "입력: ");
    move(input_y+3, input_x + 7);

    refresh();

    timeout(-1);
    getnstr(input, 49);

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
    napms(10000);
}