#include "json_topic.h"
#include <string.h>
#include <stdio.h>
#include "../global.h"

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

// 문자열 내 "action": 키를 찾아 해당 ActionType 반환
ActionType parse_action_from_json(const char *json_str) {
    if (strstr(json_str, "CHARGE_WEAK")) return ACTION_CHARGE_WEAK;
    if (strstr(json_str, "CHARGE_STRONG")) return ACTION_CHARGE_STRONG;
    if (strstr(json_str, "ATTACK")) return ACTION_ATTACK;
    if (strstr(json_str, "BLOCK")) return ACTION_BLOCK;
    if (strstr(json_str, "COUNTER")) return ACTION_COUNTER;
    return ACTION_NONE;
}

// "timestamp":123456789 구문에서 숫자 추출
long long parse_timestamp_from_json(const char *json_str) {
    long long ts = 0;
    char *ptr = strstr(json_str, "\"timestamp\":");
    if (ptr) sscanf(ptr, "\"timestamp\":%lld", &ts);
    return ts;
}

void parse_nickname_from_json(const char *json_str, char *output_buf, int bufsize) {
    char *start = strstr(json_str, "\"nickname\":\"");
    if (start) {
        start += strlen("\"nickname\":\"");
        char *end = strchr(start, '\"');
        if (end && end - start < bufsize) {
            strncpy(output_buf, start, end - start);
            output_buf[end - start] = '\0';
        } else {
            strncpy(output_buf, "Unknown", bufsize);
        }
    } else {
        strncpy(output_buf, "Unknown", bufsize);
    }
}

// 서버 → 클라이언트: 상태 응답 JSON 문자열 생성
void make_status_response(char *buffer, PlayerState *self, PlayerState *opponent, const char *event_msg) {
    snprintf(buffer, BUFFER_SIZE,
        "{\"self\":{\"data\":%d,\"max_data\":%d,\"charged_attack\":%d,\"nickname\":\"%s\"},"
        "\"opponent\":{\"data\":%d,\"max_data\":%d,\"charged_attack\":%d,\"nickname\":\"%s\"},"
        "\"event\":\"%s\"}",
        self->data, self->max_data, self->charged_attack, self->nick,
        opponent->data, opponent->max_data, opponent->charged_attack, opponent->nick,
        event_msg
    );
}
