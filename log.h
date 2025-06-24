#ifndef LOG_H
#define LOG_H

#include <stdio.h>

// 로그 파일 작성 함수
void write_log_file(const char *upgrade, const char *logfilename);
// 로그 파일 읽기 함수 // 최근 5개 + 없음 표시 가능 + 닉네임 확인후 조치
void read_log_file(const char *logfilename, int y_offset,int x_offset);

#endif 
