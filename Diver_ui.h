#ifndef DIVER_UI_H
#define DIVER_UI_H

#include <ncurses.h>
#include <stdlib.h>


//  UI 관련 함수 선언
void draw_border(int mode);   // 테두리 그리기
void draw_game_time(); //시간
void size_check();

//초기화
void reset_stat();

//screen
void start_screen(const char *client_name); // 시작 화면
void guide_screen();
void loading_screen_frame(int waiting); //수정

// 일시 정지 화면
void pause_screen();  
int pause_choice();

//엔딩화면
void winner_ending_screen(); 
void loser_ending_screen();

#endif
