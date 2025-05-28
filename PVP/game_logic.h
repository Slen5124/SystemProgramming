#include <stdbool.h> 

#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include "shared_eco.h"

// 행동 처리 함수: 행동 주체, 대상, 입력된 행동
void process_action(PlayerState *actor, PlayerState *opponent, ActionType action);

void check_and_apply_actions(PlayerState*, PlayerState*);

// 게임 종료 여부 확인 (DATA가 0 이하인 경우)
bool is_game_over(PlayerState *p1, PlayerState *p2);

// 승자 반환: 0 또는 1, 무승부는 -1
int get_winner(PlayerState *p1, PlayerState *p2);

// 즉시 행동 적용 함수 (PVP.c 에서 직접 호출)
void apply_action(PlayerState *actor, PlayerState *opponent);

#endif
