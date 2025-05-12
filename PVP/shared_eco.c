// shared_eco.c
#include "shared_eco.h"
#include <time.h>

long long get_current_time_ms() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000LL + ts.tv_nsec / 1000000LL;
}

long long get_action_delay(ActionType action) {
    switch (action) {
        case ACTION_CHARGE_WEAK: return 3000;  // 3초
        case ACTION_CHARGE_STRONG: return 5000; // 5초
        case ACTION_FIRE: return 1000;         // 1초
        case ACTION_BLOCK: return 2000;        // 2초
        case ACTION_COUNTER: return 500;       // 0.5초
        default: return 0;
    }
}
