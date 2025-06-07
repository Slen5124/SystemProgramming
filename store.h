#ifndef STORE_UI_H
#define STORE_UI_H

void call_store(int time_limit); 

int store_menu_ui(int time_limit,time_t start); //선택
void draw_store_ui(int highlight,int timeleft); //테두리랑 timeout
// 상점 기능

void rand_ability_no_dup(); //중복방지 item
void ability_upgrade(int ability_sort); 

void handle_buy(int choice); //구매 
#endif
