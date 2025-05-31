#include <stdio.h>    // printf(), 기본 입출력
#include <locale.h>   // setlocale(), 로케일 설정
#include <stdlib.h>   // exit() 같은 함수 (필요할 경우)
#include <ncurses.h>
#include <signal.h>
#include <string.h>
#include "store.h"  // 다른 함수들이 선언된 헤더
#include "Diver_ui.h"
#include "global.h"


void sig_pause(int signal){
    if(Player.pause_access){pause_screen();}
    else{printf("전투 중에 정지가 불가능합니다.\n");} // PVP전투중에 pause 눌렀을 때.
}

int main() {
    //struct termios settings; //컨트롤 s 정지버튼 기능제거
    //tcgetattr(STDIN_FILENO, &settings);
    //settings.c_iflag &= ~IXON; //
    //tcsetattr(STDIN_FILENO, TCSANOW, &settings);

    signal(SIGQUIT, sig_pause); // SIGQUIT (Ctrl+\)를 PAUSE로
    signal(SIGINT, SIG_IGN);  // SIGINT (Ctrl+C)도 무시
    
    setlocale(LC_ALL, "ko_KR.utf-8"); 
    initscr();
    size_check();
    endwin();

    printf("Enter your nickname: ");
    scanf("%31s",Player.nick);
    initscr();

   
    start_screen(Player.nick);  // 시작 화면 띄움
    guide_screen();

    call_store(100);
    
    loading_screen(1);
    //reset_stat();
    endwin();
    return 0;

}
