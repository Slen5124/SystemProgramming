#ifndef STORE_UI_H
#define STORE_UI_H

int store_menu_ui(); //선택
void draw_store_ui(int highlight,int timeleft); //테두리랑 timeout
// 상점 기능

void handle_upgrade(int choice); 
void save_upgrade(const char *upgrade);

#endif