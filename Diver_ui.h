#ifndef DIVER_UI_H
#define DIVER_UI_H

#include <ncurses.h>
#include <stdlib.h>


// ✅ UI 관련 함수 선언
void draw_border(int mode);   // 테두리 그리기
void draw_game_time();
void size_check();
void reset_stat();

//screen
void start_screen(const char *client_name); // 시작 화면
void guide_screen();
void loading_screen(int waiting);

void pause_screen();  // 일시 정지 화면
int pause_choice();

void winner_ending_screen(); //엔딩화면면
void loser_ending_screen();

#endif
