#include "global.h"
#include <stdbool.h>

// 게임상태
int pause_access = 1;
int store_access = 1;
int HEIGHT = 35;
int WIDTH = 130;
int winning_streak = 0;




//플레이어 데이터
int data = 200;
int atk_stat =20;
int dfs_stat = 20;
int pve_start_bit =3;
float pve_strong_atk_stat = 5.0 ;

int buy_atk_cnt =1;
int buy_dfs_cnt =1;
int ability_sort = -1;
bool ability_dup_check[10] = {false};