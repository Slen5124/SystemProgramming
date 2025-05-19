#ifndef DIVER_UI_H
#define DIVER_UI_H

#include <ncurses.h>

// ✅ UI 관련 함수 선언
void draw_border(int mode);   // 테두리 그리기
void draw_game_time();

void pause_screen(int store_access1);  // 일시 정지 화면
int pause_choice(int store_access1);

void start_screen(const char *client_name); // 시작 화면
int store_menu_ui(); // 상점 메뉴 UI

#endif