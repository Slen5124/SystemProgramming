#include "Pve_game.h"

// 전역 변수 정의
Entity player = {100, 20, 20, 100, 20};
Entity monster = {10, 5, 10, 50, 10};

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

int main() {
    setlocale(LC_ALL, "");
    initscr();
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);    // 딜레이 시 (붉은 테두리)
    init_pair(2, COLOR_GREEN, COLOR_BLACK);  // 딜레이 시 (초록 테두리)

    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(FALSE);
    srand(time(NULL));

    attron(COLOR_PAIR(2));
    box(stdscr, 0, 0);
    attroff(COLOR_PAIR(2));
    refresh();

    int turn = 0;
    int round = 1;
    int selected_action = 0;
    char player_action_result[100] = "";
    char monster_action_result[100] = "";
    int boss_count = 0;

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
                } else {
                    snprintf(player_action_result, 100, "BIT 부족!");
                    attron(COLOR_PAIR(1));
                    box(stdscr, 0, 0);
                    attroff(COLOR_PAIR(1));
                }
                break;
            case 1:
                if (player.bit >= 5) {
                    monster.data -= player.strong_attack;
                    player.bit -= 5;
                    snprintf(player_action_result, 100, "강화 공격!");
                } else {
                    snprintf(player_action_result, 100, "BIT 부족!");
                    attron(COLOR_PAIR(1));
                    box(stdscr, 0, 0);
                    attroff(COLOR_PAIR(1));
                }
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
                } else {
                    snprintf(player_action_result, 100, "BIT 부족!");
                    attron(COLOR_PAIR(1));
                    box(stdscr, 0, 0);
                    attroff(COLOR_PAIR(1));
                }
                break;
        }

        monster_turn(&monster, &player, monster_action_result, round, turn, selected_action);

        turn++;

        if (monster.data <= 0) {
            if (round % 7 == 0) { // 보스 라운드라면
                bonus_round(boss_count, &player);
                boss_count++;   
            }

            round++;
            monster.data = DATA_BAR_WIDTH;
            monster.bit = 5;
            turn = 0;

            erase();

            attron(COLOR_PAIR(2));
            box(stdscr, 0, 0);
            attroff(COLOR_PAIR(2));

            if(round % 7 == 0){
                mvprintw(TERM_HEIGHT / 2, (TERM_WIDTH - 30) / 2, "위험!! ** 보스 출현 **");
            } else {
                mvprintw(TERM_HEIGHT / 2, (TERM_WIDTH - 30) / 2, "새로운 적이 등장했습니다!");
            }
            refresh();
            napms(1500);
        }
    }

    erase();
    attron(COLOR_PAIR(2));
    box(stdscr, 0, 0);
    attroff(COLOR_PAIR(2));
    
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