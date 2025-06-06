#ifndef GAME_H
#define GAME_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <string.h>
#include <time.h>
#include <locale.h>
#include <wchar.h>
#include <ncursesw/ncurses.h>
#include <sys/time.h>
#include <poll.h>
#include "global.h"


#define BIT_BAR_WIDTH 10
#define TERM_HEIGHT 35
#define TERM_WIDTH 130
#define FILLED_CHR "█"

extern int Player_DATA_BAR_WIDTH;
extern int ROUND_MON_NO;

extern PlayerState Player;

typedef struct {
    char *name;
    char *art[9];
    int max_data;      // 최대 데이터 (체력)
    int data;          // 현재 데이터
    int bit;           // 현재 BIT
    int attack;        // 기본 공격력
    int strong_attack; // 강화 공격력
    int defense;       // 방어력
} MonsterInfo;

// 전역 변수
//extern Entity player;//$$$$$$$$$$$$$$ 병조 Player 구조체 변수 사용했으니까 참고고해서 변경
extern MonsterInfo monsters[];
extern MonsterInfo current_monster; // 현재 전투 중인 몬스터

// UI 관련 함수
void draw_data_bar(int y, int x, int data, int width);
void draw_bit_bar(int y, int x, int bit, int width);
void print_status(int turn, int remaining_time, int round, PlayerState Player , MonsterInfo monster, int selected_action);
void draw_ui(PlayerState Player, MonsterInfo monster, int round,int monster_No);

// 게임 로직 함수
void monster_turn(MonsterInfo *monster, PlayerState *Player, char *monster_action_result, int round, int turn, int selected_action,int monster_No);
int wait_for_input_with_timeout(int *selected_action, int timeout_sec, int turn, int round, PlayerState Player, MonsterInfo monster, char *player_action_result, char *monster_action_result,int monster_No);
void bonus_round(int boss_count, PlayerState *Player);

// 게임 초기화 및 종료 관련
void initialize_game(void);


// 게임 플레이 관련
void game_loop(void);
void handle_player_action(int selected_action, char* player_action_result,int monster_No);
void handle_round_end(int* round, int* turn, int* cure_data, int* boss_count, 
                     int bonus_rand, int* monster_No);
void display_victory_screen(int round, int cure_data);

#endif 