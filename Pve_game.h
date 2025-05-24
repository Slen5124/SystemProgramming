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

#define DATA_BAR_WIDTH 10
#define BIT_BAR_WIDTH 10
#define TERM_HEIGHT 35
#define TERM_WIDTH 130
#define FILLED_CHR "█"

typedef struct {
    int data;
    int bit;
    int attack;
    int strong_attack;
    int defense;
} Entity;

typedef struct {
    char *name;
    char *art[5];
} MonsterInfo;

// 전역 변수
extern Entity player;
extern Entity monster;
extern MonsterInfo monsters[];

// UI 관련 함수
void draw_data_bar(int y, int x, int data, int width);
void draw_bit_bar(int y, int x, int bit, int width);
void print_status(int turn, int remaining_time, int round, Entity player, Entity monster, int selected_action);
void draw_ui(Entity player, Entity monster, int round);

// 게임 로직 함수
void monster_turn(Entity *monster, Entity *player, char *monster_action_result, int round, int turn, int selected_action);
int wait_for_input_with_timeout(int *selected_action, int timeout_sec, int turn, int round, Entity player, Entity monster, char *player_action_result, char *monster_action_result);
void bonus_round(int boss_count, Entity *player);

#endif