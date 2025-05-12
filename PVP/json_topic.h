#ifndef JSON_UTIL_H
#define JSON_UTIL_H

#include "shared_eco.h"

// 클라이언트 → 서버
ActionType parse_action_from_json(const char *json_str);
long long parse_timestamp_from_json(const char *json_str);

// 서버 → 클라이언트
void make_status_response(char *buffer, PlayerState *self, PlayerState *opponent, const char *event_msg);
void parse_nickname_from_json(const char *json_str, char *output_buf, int bufsize);

#endif
