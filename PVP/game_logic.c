#include "game_logic.h"
#include "shared_eco.h"
#include <stdio.h>

// 내부 함수: 딜레이가 끝난 행동 실제 적용
static void apply_action(PlayerState *actor, PlayerState *opponent) {
    ActionType action = actor->current_action;

    switch (action) {
        case ACTION_CHARGE_WEAK:
            actor->charged_attack += actor->attack_power * 3;
            break;
        case ACTION_CHARGE_STRONG:
            actor->charged_attack += actor->attack_power * 5;
            break;
        case ACTION_ATTACK:
            if (opponent->is_blocking && get_current_time_ms() <= opponent->block_end_ms) {
                // 공격이 막힘
            } else if (opponent->is_counter_ready &&
                       get_current_time_ms() - opponent->counter_window_start_ms <= COUNTER_WINDOW) {
                opponent->data -= actor->charged_attack; // 카운터 성공
            } else {
                opponent->data -= actor->charged_attack; // 일반 공격 적용
            }
            actor->charged_attack = 0;
            break;
        case ACTION_BLOCK:
            actor->is_blocking = 1;
            actor->block_end_ms = get_current_time_ms() + BLOCK_DURATION;
            break;
        case ACTION_COUNTER:
            // 실제 효과는 공격 타이밍에 반영됨
            break;
        default:
            break;
    }

    actor->current_action = ACTION_NONE;
    actor->is_in_delay = 0;
}

void process_action(PlayerState *actor, PlayerState *opponent, ActionType action) {
    long long now = get_current_time_ms();

    // 딜레이 중이면 무시
    if (actor->is_in_delay && now < actor->delay_until_ms) return;

    // 카운터는 타이밍 기준 특별 처리
    if (action == ACTION_COUNTER) {
        actor->is_counter_ready = 1;
        actor->counter_window_start_ms = now;
        actor->is_in_delay = 1;
        actor->delay_until_ms = now + COUNTER_WINDOW;
        actor->current_action = action;
        return;
    }

    // 행동 예약 + 딜레이 시작
    actor->current_action = action;
    actor->is_in_delay = 1;
    switch (action) {
        case ACTION_CHARGE_WEAK:
            actor->delay_until_ms = now + DELAY_CHARGE_WEAK;
            break;
        case ACTION_CHARGE_STRONG:
            actor->delay_until_ms = now + DELAY_CHARGE_STRONG;
            break;
        case ACTION_ATTACK:
            actor->delay_until_ms = now + DELAY_ATTACK;
            break;
        case ACTION_BLOCK:
            actor->delay_until_ms = now + DELAY_BLOCK;
            break;
        default:
            actor->delay_until_ms = now + 1000; // fallback delay
            break;
    }
}

int is_game_over(PlayerState *p1, PlayerState *p2) {
    return (p1->data <= 0 || p2->data <= 0);
}

int get_winner(PlayerState *p1, PlayerState *p2) {
    if (p1->data <= 0 && p2->data <= 0) return -1; // 무승부
    if (p1->data <= 0) return 1;
    if (p2->data <= 0) return 0;
    return -1; // 게임 진행 중
}

// 외부에서 주기적으로 호출 필요
void check_and_apply_actions(PlayerState *p1, PlayerState *p2) {
    long long now = get_current_time_ms();

    if (p1->is_in_delay && now >= p1->delay_until_ms) {
        apply_action(p1, p2);
    }
    if (p2->is_in_delay && now >= p2->delay_until_ms) {
        apply_action(p2, p1);
    }

    // 카운터 실패 처리
    if (p1->current_action == ACTION_COUNTER &&
        p1->is_counter_ready && now >= p1->delay_until_ms) {
        p1->data -= COUNTER_FAIL_DAMAGE;
        p1->is_counter_ready = 0;
        p1->current_action = ACTION_NONE;
        p1->is_in_delay = 0;
    }
    if (p2->current_action == ACTION_COUNTER &&
        p2->is_counter_ready && now >= p2->delay_until_ms) {
        p2->data -= COUNTER_FAIL_DAMAGE;
        p2->is_counter_ready = 0;
        p2->current_action = ACTION_NONE;
        p2->is_in_delay = 0;
    }
}
