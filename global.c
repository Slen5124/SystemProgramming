#include "global.h"
#include <stdbool.h>
#include <time.h>

long long get_current_time_ms() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000LL + ts.tv_nsec / 1000000LL;
}

long long get_action_delay(ActionType action) {
    switch (action) {
        case ACTION_CHARGE_WEAK: return 3000;  // 3초
        case ACTION_CHARGE_STRONG: return 5000; // 5초
        case ACTION_ATTACK:return 1000;
        case ACTION_BLOCK: return 2000;        // 2초
        case ACTION_COUNTER: return 500;       // 0.5초
        default: return 0;
    }
}


// 전역 플레이어 변수 정의
PlayerState Player = {
    .pause_access = 1,
    .store_access = 0,
    .winning_streak = 3,
    .start_time = 0,

    .id = 0,
    .nick = "",  // 문자열 배열 초기화
    .data = 200,
    .bit = 5,//5로 수정 -원준
    .atk_stat = 20,
    .dfs_stat = 20,
    .pve_start_bit = 5,
    .pve_data_intake = 50,
    .pve_strong_atk_stat = 5,

    .pvp_charge_minus = 0,
    .pvp_counter_atk_power_stat = 1.0,
    .pvp_charge_strong = 1.0,

    .charged_attack = 0,
    .defense_shield = 0,
    .is_in_delay = 0,
    .delay_until_ms = 0,
    .is_counter_ready = 0,
    .counter_window_start_ms = 0,
    //.current_action = ACTION_NONE,  // 필요하면 추가
    .block_end_ms = 0,

    .buy_atk_cnt = 1,
    .buy_dfs_cnt = 1,

    .ability_sort = -1,
    .ability_dup_check = {false}
};
