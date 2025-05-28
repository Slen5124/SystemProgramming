#ifndef STORE_UI_H
#define STORE_UI_H

void call_store(int time_limit); //main에서 store call

int store_menu_ui(int time_limit); //선택
void draw_store_ui(int highlight,int timeleft); //테두리랑 timeout
// 상점 기능

void rand_ability_no_dup();
void ability_upgrade(int ability_sort);
void handle_buy(int choice); 
#endif
