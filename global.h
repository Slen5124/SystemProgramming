#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdbool.h>
#include <time.h>  // 🔥 시간 계산을 위해 추가
// 게임상태
extern int pause_access;
extern int store_access;
extern int HEIGHT;
extern int WIDTH;
extern time_t start_time;
extern int winning_streak;





//플레이어 데이터ㄴ
extern int data;
extern int atk_stat;
extern int dfs_stat;

extern int pve_start_bit;
extern int pve_data_intake;
extern int pve_strong_atk_stat;

extern int pvp_charge_minus;
extern float pvp_counter_atk_power_stat;
extern float pvp_charge_strong;

extern int buy_atk_cnt;
extern int buy_dfs_cnt;

extern int ability_sort;
extern bool ability_dup_check[7];


#endif
