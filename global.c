#include "global.h"
#include <stdbool.h>

// 게임상태
int pause_access = 1;
int store_access = 1;
int HEIGHT = 35;
int WIDTH = 130;
int winning_streak = 3;




//플레이어 데이터
int data = 500;
int atk_stat =20;
int dfs_stat = 20;

int pve_start_bit =3;  //+1, 1.5qo, 1.2qo
int pve_data_intake =50;
int pve_strong_atk_stat = 5 ;

//차지속도, 카운터공격계수, 강한차지 충전량
int pvp_charge_minus = 0;
float pvp_counter_atk_power_stat = 1.0;
float pvp_charge_strong = 1.0;

int buy_atk_cnt =1;
int buy_dfs_cnt =1;
int ability_sort = -1;

bool ability_dup_check[7] = {false};

