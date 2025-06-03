#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdbool.h>
#include <time.h>
//#include "shared_eco.h" // 이미 ActionType이 정의되어 있음

// 게임 화면 크기
#define HEIGHT 35
#define WIDTH 130


typedef enum {
    ACTION_NONE,
    ACTION_ATTACK,
    ACTION_BLOCK,
    ACTION_CHARGE_WEAK,
    ACTION_CHARGE_STRONG,
    ACTION_COUNTER
} ActionType;

// 플레이어 상태 구조체 정의
typedef struct {
    // 게임 상태
    int pause_access;
    int store_access;
    int winning_streak;
    time_t start_time;

    // 플레이어 데이터
    int id;
    char nick[32];  // 일반 문자열 배열로 수정
    int data;
    int bit;//수정-원준
    int max_data;
    int atk_stat;
    int dfs_stat;
    int pve_start_bit;
    int pve_data_intake;
    int pve_strong_atk_stat;

    // PVP 관련 데이터
    int pvp_charge_minus;
    float pvp_counter_atk_power_stat;
    float pvp_charge_strong;

    int charged_attack;
    int defense_shield;  // BLOCK 시 부여되는 방어 수치
    int is_in_delay;
    long long delay_until_ms;
    int is_counter_ready;
    long long counter_window_start_ms;
    ActionType current_action; // shared_eco.h에서 가져옴
    long long block_end_ms;

    // 상점 관련 데이터
    int buy_atk_cnt;
    int buy_dfs_cnt;

    // 능력 관련 데이터
    int ability_sort;
    bool ability_dup_check[7];

} PlayerState;

// 전역 플레이어 변수 선언 (실제 정의는 다른 C 파일에서)
extern PlayerState Player;

#define DELAY_ATTACK 1000
#define DELAY_BLOCK  2000
#define DELAY_CHARGE_WEAK 800
#define DELAY_CHARGE_STRONG 1600
#define COUNTER_WINDOW  500
#define BLOCK_DURATION  2000

long long get_action_delay(ActionType action);
long long get_current_time_ms(void);

#endif // GLOBAL_H
