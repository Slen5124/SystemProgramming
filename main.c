#include <stdio.h>    // printf(), 기본 입출력
#include <locale.h>   // setlocale(), 로케일 설정
#include <stdlib.h>   // exit() 같은 함수 (필요할 경우)
#include <ncurses.h>
#include <signal.h>
#include "store.h"  // 다른 함수들이 선언된 헤더
#include "Diver_ui.h"
#include "global.h"

void sig_pause(int signal){
    if(pause_access){pause_screen(store_access);}
    //else // PVP전투중에 pause 눌렀을 때.
}

int main() {
    //struct termios settings; //컨트롤 s 정지버튼 기능제거
    //tcgetattr(STDIN_FILENO, &settings);
    //settings.c_iflag &= ~IXON; //
    //tcsetattr(STDIN_FILENO, TCSANOW, &settings);


    signal(SIGQUIT, sig_pause); // SIGQUIT (Ctrl+\)를 PAUSE로
    //signal(SIGINT, SIG_IGN);  // SIGINT (Ctrl+C)도 무시
    

    setlocale(LC_ALL, "ko_KR.utf-8");
    initscr();
    if (COLS < WIDTH || LINES < HEIGHT) {
    endwin();
    printf("🚨 DIVER : ONELIFE의 화면은 %dx%d으로 제작되었습니다. 터미널 크기를 %dx%d이상으로 설정하세요!\n",WIDTH, HEIGHT, WIDTH, HEIGHT);
    exit(1);
    }

    const char *client_name = "Player001";
    start_screen(client_name);  // 시작 화면 띄움
    start_time = time(NULL);

    int cnt = 0;
    while(cnt++<5){
        int choice = store_menu_ui(); // 상점 기능 호출
        handle_upgrade(choice);    // 강화 기능 수행
    }
    endwin();
    return 0;

}
