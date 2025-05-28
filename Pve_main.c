#include "Pve_game.h"
#include "Diver_ui.h"
#include "global.h"
#include "store.h"
#include "log.h"

// 전역 변수 정의
int Player_DATA_BAR_WIDTH = 200;
int monster_DATA_BAR_WIDTH = 50;
int boss_DATA_BAR_WIDTH = 200;

Entity player = {200, 5, 100, 100, 20}; // data, bit, attack, strong_attack, defense
Entity monster = {50, 5, 10, 50, 10};
Entity boss = {200, 5, 20, 100, 20};

int main() {
    initialize_game();
    game_loop();
    display_game_end();
    endwin();
    return 0;
}