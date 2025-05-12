#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>

// 최대 데이터량 (DATA = HP)
#define MAX_DATA 100

// 행동 타입 열거
typedef enum {
    ACTION_NONE,
    ACTION_CHARGE_WEAK,    // Ctrl+C (딜레이 3초)
    ACTION_CHARGE_STRONG,  // Ctrl+A (딜레이 5초)
    ACTION_ATTACK,         // Ctrl+V (딜레이 1초)
    ACTION_BLOCK,          // Ctrl+Z (딜레이 2초 + 3초 지속)
    ACTION_FIRE,
    ACTION_COUNTER         // Ctrl+S (0.5초 내 반응)

} ActionType;

// 행동별 고정 딜레이 (ms)
#define DELAY_CHARGE_WEAK     3000
#define DELAY_CHARGE_STRONG   5000
#define DELAY_ATTACK          1000
#define DELAY_BLOCK           2000
#define BLOCK_DURATION        3000
#define COUNTER_WINDOW        500   // 0.5초 이내 입력 시 성공
#define COUNTER_FAIL_DAMAGE   3

// 플레이어 상태 구조체
typedef struct {
    int id;                    // 0 또는 1
    char nickname[32];
    int data;                  // DATA = HP
    int attack_power;
    int defense_power;
    int charged_attack;

    int atk;                // 기본 공격력
    int def;                // 기본 방어력

    int is_in_delay;
    long long delay_until_ms;

    int is_blocking;
    long long block_end_ms;

    int is_counter_ready;
    long long counter_window_start_ms;

    ActionType current_action;
} PlayerState;

// 현재 시간(ms) 반환 함수
long long get_current_time_ms();
long long get_action_delay(ActionType action);

#endif
