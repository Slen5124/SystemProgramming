#include "Pve_game.h"

void draw_data_bar(int y, int x, int data, int width) {
    mvprintw(y, x, "DATA : ");
    for (int i = 0; i < width; i++) {
        if (i < data) addstr(FILLED_CHR);
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

void print_status(int turn, int remaining_time, int round, Entity player, Entity monster, int selected_action) {
    mvprintw(1, 1, "라운드: %d", round);
    mvprintw(2, 1, "턴: %d | 남은 시간: %d초", turn, remaining_time);

    const char *actions[] = {
        "공격 (1 BIT)",
        "강화 공격 (3 BIT)",
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

void draw_ui(Entity player, Entity monster, int round) {
    MonsterInfo current_monster;

    if(round % 7 == 0){
        current_monster = monsters[3];
    } else {
        current_monster = monsters[(round - 1) % 3];
    }
    
    // 몬스터 UI출력
    for (int i = 0; i < 5; i++) {
        mvprintw(6 + i, 90, "%s", current_monster.art[i]);
    }
    mvprintw(10, 90, "👾 %s", current_monster.name);
    draw_data_bar(12, 90, monster.data, DATA_BAR_WIDTH);
    draw_bit_bar(13, 90, monster.bit, BIT_BAR_WIDTH);
    mvprintw(16, 90, "공격력 : %d      방어력 : %d", monster.attack, monster.defense);
    
    // 플레이어 UI출력
    mvprintw(17, 5, "  (\\_/) ");
    mvprintw(18, 5, " ( •_•)");
    mvprintw(19, 5, "/ >🔥> ");
    mvprintw(21, 5, "🧑 플레이어");
    draw_data_bar(23, 5, player.data, DATA_BAR_WIDTH);
    draw_bit_bar(24, 5, player.bit, BIT_BAR_WIDTH);
    mvprintw(27, 5, "공격력 : %d      방어력 : %d", player.attack, player.defense);
}