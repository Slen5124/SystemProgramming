#ifndef GLOBAL_H
#define GLOBAL_H

#include <time.h>  // 🔥 시간 계산을 위해 추가
// 게임상태
extern int pause_access;
extern int store_access;
extern int HEIGHT;
extern int WIDTH;
extern time_t start_time;





//플레이어 데이터
extern int data;
extern int atk_stat;
extern int dfs_stat;
extern int pve_start_bit;
extern float pve_strong_atk_stat;
extern int buy_cnt;

#endif