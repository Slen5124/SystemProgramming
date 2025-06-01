#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "game_logic.h"
#include "../global.h"

// 즉시 적용
void apply_action(PlayerState *actor, PlayerState *opponent) {
    long long now = get_current_time_ms();
    switch (actor->current_action) {
        case ACTION_CHARGE_WEAK:
            actor->charged_attack += actor->atk_stat * 3;
            break;

        case ACTION_CHARGE_STRONG:
            actor->charged_attack += actor->atk_stat * 5;
            break;

        case ACTION_ATTACK: {
            int dmg = actor->charged_attack;
            actor->charged_attack = 0;

            // 1) 방어막 우선 적용
            if (opponent->defense_shield > 0 && now <= opponent->block_end_ms) {
                int sh = opponent->defense_shield;
                dmg = (dmg > sh ? dmg - sh : 0);
                opponent->defense_shield = 0;
            }
            // 2) 카운터 성공 판정
            else if (opponent->is_counter_ready &&
                     now - opponent->counter_window_start_ms <= COUNTER_WINDOW) {
                // 반사
                opponent->data -= dmg;
                opponent->is_counter_ready = 0;
                opponent->current_action   = ACTION_NONE;
                opponent->is_in_delay      = 0;
                return;
            }
            // 3) 일반 데미지
            opponent->data -= dmg;
            break;
        }

        case ACTION_COUNTER:
            // 카운터 동작은 check_and_apply_actions에서 처리
            return;

        default:
            break;
    }

    // ACTION_COUNTER일 때는 위에서 return되어 이 코드를 건너뜁니다
    actor->current_action = ACTION_NONE;
    actor->is_in_delay    = 0;
}

// 입력 처리
void process_action(PlayerState *actor, PlayerState *opponent, ActionType action) {
    long long now = get_current_time_ms();
    if (actor->is_in_delay && now < actor->delay_until_ms) return;

    // COUNTER 준비
    if (action == ACTION_COUNTER) {
        actor->is_counter_ready        = 1;
        actor->counter_window_start_ms = now;
        actor->is_in_delay             = 1;
        actor->delay_until_ms          = now + COUNTER_WINDOW;
        actor->current_action          = ACTION_COUNTER;
        return;
    }

    // BLOCK 즉시 방어막, 2초 딜레이
    if (action == ACTION_BLOCK) {
        actor->defense_shield = actor->dfs_stat * 3;
        actor->block_end_ms   = now + BLOCK_DURATION;
        actor->is_in_delay    = 1;
        actor->delay_until_ms = now + DELAY_BLOCK;
        actor->current_action = ACTION_NONE;
        return;
    }

    // 나머지 행동 예약
    actor->current_action = action;
    actor->is_in_delay    = 1;
    switch (action) {
        case ACTION_CHARGE_WEAK:
            actor->delay_until_ms = now + DELAY_CHARGE_WEAK; break;
        case ACTION_CHARGE_STRONG:
            actor->delay_until_ms = now + DELAY_CHARGE_STRONG; break;
        case ACTION_ATTACK:
            actor->delay_until_ms = now + DELAY_ATTACK; break;
        default:
            actor->delay_until_ms = now + 1000; break;
    }
}

// 게임 종료 체크
bool is_game_over(PlayerState *p1, PlayerState *p2) {
    bool over = (p1->data <= 0) || (p2->data <= 0);
    fprintf(stderr, "[DBG is_game_over] p1.data=%d, p2.data=%d → %s\n",
            p1->data, p2->data, over ? "TRUE" : "FALSE");
    return over;
}

// 승자 판단
int get_winner(PlayerState *p1, PlayerState *p2) {
    if (p1->data <= 0 && p2->data <= 0) {
        fprintf(stderr, "[DBG get_winner] DRAW\n");
        return -1;  // draw은 -1로 통일
    }
    if (p1->data <= 0) {
        fprintf(stderr, "[DBG get_winner] P2 wins\n");
        return 1;
    }
    if (p2->data <= 0) {
        fprintf(stderr, "[DBG get_winner] P1 wins\n");
        return 0;
    }
    return -2;  // not over
}

// 매 틱 호출
void check_and_apply_actions(PlayerState *p1, PlayerState *p2) {
    long long now = get_current_time_ms();

    // 1) p1 행동부터 적용, 죽으면 p2 행동 skip
    if (p1->is_in_delay && now >= p1->delay_until_ms) {
        apply_action(p1, p2);
        if (p2->data <= 0) return;
    }
    // 2) p2 행동 적용
    if (p2->is_in_delay && now >= p2->delay_until_ms) {
        apply_action(p2, p1);
        if (p1->data <= 0) return;
    }

    // 3) 카운터 실패 처리
    if (p1->current_action == ACTION_COUNTER &&
        p1->is_counter_ready && now >= p1->delay_until_ms) {
        // 실패 시 상대 공격력 피해
        p1->data -= p2->atk_stat;
        p1->is_counter_ready = 0;
        p1->current_action   = ACTION_NONE;
        p1->is_in_delay      = 0;
    }
    if (p2->current_action == ACTION_COUNTER &&
        p2->is_counter_ready && now >= p2->delay_until_ms) {
        p2->data -= p1->atk_stat;
        p2->is_counter_ready = 0;
        p2->current_action   = ACTION_NONE;
        p2->is_in_delay      = 0;
    }
}
