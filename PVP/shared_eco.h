#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>

// 최대 데이터량 (DATA = HP)
#define MAX_DATA 100

typedef enum {
    ACTION_NONE,
    ACTION_CHARGE_WEAK,
    ACTION_CHARGE_STRONG,
    ACTION_ATTACK,
    ACTION_BLOCK,
    ACTION_FIRE,
    ACTION_COUNTER
} ActionType;

// 딜레이(밀리초)
#define DELAY_CHARGE_WEAK   3000
#define DELAY_CHARGE_STRONG 5000
#define DELAY_ATTACK        1000
#define DELAY_BLOCK         2000
#define BLOCK_DURATION      3000
#define COUNTER_WINDOW      500
#define COUNTER_FAIL_DAMAGE 3

typedef struct {
   int id;
   char nickname[32];
   int data;
   int attack_power;
   /int defense_power;
    int charged_attack;

    int defense_shield;      // ★ BLOCK 시 부여되는 방어 수치

    int is_in_delay;
    long long delay_until_ms;

    int is_counter_ready;
    long long counter_window_start_ms;

    ActionType current_action;
    long long block_end_ms;
} PlayerState;

// 시간(ms)
long long get_current_time_ms();

#endif
