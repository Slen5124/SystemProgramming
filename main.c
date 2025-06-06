#include <stdio.h>    // printf(), 기본 입출력
#include <locale.h>   // setlocale(), 로케일 설정
#include <stdlib.h>   // exit() 같은 함수 (필요할 경우)
#include <ncurses.h>  // 화면출력 ncurses
#include <signal.h>
#include <string.h>
#include <termios.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "store.h"  // 다른 함수들이 선언된 헤더
#include "Diver_ui.h"
#include "global.h"
#include "log.h"
#include "Pve_game.h"
#include "PVP/json_topic.h"

#define DEFAULT_SERVER_IP "127.0.0.1"
#define PORT 5124


int run_pvp_mode(int sock);


int Player_DATA_BAR_WIDTH = 200; // 원준이 전역변수
int ROUND_MON_NO=0; // 라운드에 처치한 몬스터 수 인듯?

extern MonsterInfo monsters[];//원준 - 수정정
/*아스키 캐릭터 몬스터들 부분 */
MonsterInfo current_monster;

void endwin_wrapper(void) {
    endwin();
}

void sig_pause(int signal){
    if(Player.pause_access){pause_screen();}
    else{printf("전투 중에 정지가 불가능합니다.\n");} // PVP전투중에 pause 눌렀을 때.
}

int main(int argc, char *argv[]) {

    atexit(endwin_wrapper);

    //struct termios settings; //컨트롤 s 정지버튼 기능제거
    //tcgetattr(STDIN_FILENO, &settings);
    //settings.c_iflag &= ~IXON; //
    //tcsetattr(STDIN_FILENO, TCSANOW, &settings);

    signal(SIGQUIT, sig_pause); // SIGQUIT (Ctrl+\)를 PAUSE로
    signal(SIGINT, SIG_IGN);  // SIGINT (Ctrl+C)도 무시
    signal(SIGTSTP,SIG_IGN);
    
    struct termios tt; tcgetattr(0,&tt);
    tt.c_iflag&=~(IXON|IXOFF); tcsetattr(0,TCSANOW,&tt);

    const char *server_ip = (argc>=2 ? argv[1] : DEFAULT_SERVER_IP);


    setlocale(LC_ALL, "en_US.utf-8"); 
    initscr();
    size_check();
    endwin();

    while(1){
        reset_stat();

        if(strcmp(Player.nick, "") == 0){
            printf("Enter your nickname: ");
            scanf("%31s",Player.nick);
        }

        int sock = socket(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in srv = {.sin_family = AF_INET, .sin_port = htons(PORT)};
        if(inet_pton(AF_INET,server_ip,&srv.sin_addr)!=1){
            perror("inet_pton"); exit(1);
        }
        printf("서버 연결 시도 중...\n");
        if(connect(sock, (struct sockaddr*)&srv, sizeof(srv)) < 0){
            perror("connect"); exit(1);
        }
        printf("connect 성공!\n");
        
        initscr();
    
        start_screen(Player.nick);  // 시작 화면 띄움
        guide_screen(); // 가이드 스크린

        Player.start_time = time(NULL);

        initialize_game();
        game_loop(); // PVE 게임루프
        Player.max_data = Player.data;
        //loading_screen(1); // PVP모드로 들어가기 위한 인터페이스를 띄운다.
        int pvp_result = run_pvp_mode(sock);
        close(sock);
        
        if (pvp_result == 1) {
            // 승리: 루프 반복
            printf("You won! Restarting from PVE mode.\n");
            continue;
        } else {
            // 무승부 또는 기타: 종료
            printf("Draw or error. Exiting.\n");
            break;
        }

    }
    
    endwin();
    return 0;

}
