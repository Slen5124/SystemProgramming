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

//#define Player_DATA_BAR_WIDTH 200
//#define monster_DATA_BAR_WIDTH 50
//#define boss_DATA_BAR_WIDTH 200
#define BIT_BAR_WIDTH 10
#define TERM_HEIGHT 35
#define TERM_WIDTH 130
#define FILLED_CHR "█"

extern int Player_DATA_BAR_WIDTH;
extern int monster_DATA_BAR_WIDTH;
extern int boss_DATA_BAR_WIDTH;


typedef struct {
    int data;
    int bit;
    int attack;
    int strong_attack;
    int defense;
} Entity;

typedef struct {
    char *name;
    char *art[9];
} MonsterInfo;

// 전역 변수
extern Entity player;
extern Entity monster;
extern Entity boss;
extern MonsterInfo monsters[];

// UI 관련 함수
void draw_data_bar(int y, int x, int data, int width);
void draw_bit_bar(int y, int x, int bit, int width);
void print_status(int turn, int remaining_time, int round, Entity player, Entity monster, int selected_action);
void draw_ui(Entity player, Entity monster, int round,int monster_No);

// 게임 로직 함수
void monster_turn(Entity *monster, Entity *player, char *monster_action_result, int round, int turn, int selected_action,int monster_No);
int wait_for_input_with_timeout(int *selected_action, int timeout_sec, int turn, int round, Entity player, Entity monster, char *player_action_result, char *monster_action_result,int monster_No);
void bonus_round(int boss_count, Entity *player);



//###########################################################################
// 게임 초기화 및 종료 관련
void initialize_game(void);
void display_game_end(void);

// 게임 플레이 관련
void game_loop(void);
void handle_player_action(int selected_action, char* player_action_result);
void handle_round_end(int* round, int* turn, int* cure_data, int* boss_count, 
                     int bonus_rand, int* monster_No);
void display_victory_screen(int round, int cure_data);

#endif