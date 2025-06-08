#include <ncurses.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <locale.h>  
#include <wchar.h>

#define Noise 40 //시작화면 0,1 갯수
#define blink_time 5 //0.1*blink_time초마다

#include "Diver_ui.h"
#include "global.h"
#include "store.h"
#include "Pve_game.h"

void draw_game_time() {
    time_t now = time(NULL);
    int elapsed = (int)(now - Player.start_time);
    int minutes = elapsed / 60;
    int seconds = elapsed % 60;
    mvprintw(1, WIDTH - 10, "[%02d:%02d]", minutes, seconds);
}

void draw_border(int mode) {
    start_color(); // 색상 초기화

    // 초록색 & 빨간색 설정
    init_pair(1, COLOR_GREEN, COLOR_BLACK); 
    init_pair(2, COLOR_RED, COLOR_BLACK);   
    init_pair(3, COLOR_WHITE, COLOR_BLACK); 

    int color = (mode == 1) ? 1 : (mode == 2) ? 2 : 3;

    attron(COLOR_PAIR(color)); // 색상 적용
    box(stdscr, 0, 0);  
    attroff(COLOR_PAIR(color)); // 색상 해제

    refresh();
}

void size_check(){
    if (COLS != WIDTH || LINES != HEIGHT) {
        endwin();
        printf("\n\n🚨 DIVER : ONELIFE의 화면은 %dx%d으로 제작되었습니다. 터미널 크기를 %dx%d으로 설정하세요!\nstty size로 현재 터미널 크기를 확인할 수 있습니다.\n\n",WIDTH, HEIGHT, WIDTH, HEIGHT);
        exit(1);
    }
}

void reset_stat() {
     FILE *file = fopen("upgrade_log.txt", "w");
    if (file != NULL) {
        fclose(file); // 파일 닫기
    }
    Player.pause_access = 1;
    Player.store_access = 0;    
    Player.start_time = 0;

    Player.id = 0;
    Player.data = 500;
    Player.max_data=500;
    Player.atk_stat = 20;
    Player.dfs_stat = 20;
    Player.pve_start_bit = 3;
    Player.pve_data_intake = 50;
    Player.pve_strong_atk_stat = 5;

    Player.pvp_charge_minus = 0;
    Player.pvp_counter_atk_power_stat = 1.0;
    Player.pvp_charge_strong = 1.0;

    Player.charged_attack = 0;
    Player.defense_shield = 0;
    Player.is_in_delay = 0;
    Player.delay_until_ms = 0;
    Player.is_counter_ready = 0;
    Player.counter_window_start_ms = 0;
    Player.block_end_ms = 0;

    Player.buy_atk_cnt = 1;
    Player.buy_dfs_cnt = 1;

    Player.ability_sort = -1;
    memset(Player.ability_dup_check, false, sizeof(Player.ability_dup_check));

    Player_DATA_BAR_WIDTH=500;
    
    monsters[0].max_data=150;
    monsters[0].data=150;
    monsters[0].bit=3;
    monsters[0].attack=20;
    monsters[0].strong_attack=100;
    monsters[0].defense=20;

    monsters[1].max_data=150;
    monsters[1].data=150;
    monsters[1].bit=3;
    monsters[1].attack=25;
    monsters[1].strong_attack=125;
    monsters[1].defense=25;

    monsters[2].max_data=200;
    monsters[2].data=200;
    monsters[2].bit=3;
    monsters[2].attack=20;
    monsters[2].strong_attack=100;
    monsters[2].defense=40;

    monsters[3].max_data=300;
    monsters[3].data=300;
    monsters[3].bit=5;
    monsters[3].attack=45;
    monsters[3].strong_attack=225;
    monsters[3].defense=45;

}


//screen
void start_screen(const char *client_name) {

    setlocale(LC_ALL, "ko_KR.UTF-8");
    initscr();

    noecho();
    curs_set(0);
    timeout(0); // non-blocking getch
    int blink = 1;
    int flip = 0;
    int ch;
    
    while (1) {
        clear();

        attron(A_BOLD);
        mvprintw(9, WIDTH / 2 - 22, "██████    ████   ██    ██   ██████   ███████ ");
        mvprintw(10,WIDTH / 2 - 22, "██   ██    ██    ██    ██   ██       ██    ██");
        mvprintw(11,WIDTH / 2 - 22, "██   ███   ██    ██    ██   ██████   ███████ ");
        mvprintw(12,WIDTH / 2 - 22, "██   ██    ██     ██  ██    ██       ██    ██");
        mvprintw(13,WIDTH / 2 - 22, "██████    ████     ████     ██████   ██     ██");
        mvprintw(16, ((WIDTH - strlen("ONE LIFE ONLINE")) / 2)-2, "ONE LIFE ONLINE");
        attroff(A_BOLD);

        // 환영 메시지
        char welcome[100];
        if (Player.winning_streak >= 1) {
            snprintf(welcome, sizeof(welcome), "[%d연승] %s, 환영합니다!", Player.winning_streak, Player.nick);
        } else {
            snprintf(welcome, sizeof(welcome), "%s, 환영합니다!", Player.nick);
        }
        mvprintw(18, (WIDTH - strlen(welcome)) / 2, "%s", welcome);

        // 깜빡이는 시작 메세지와 테두리
        if (blink>1) {attron(A_REVERSE);} // 반전 효과 적용
        mvprintw(19, (WIDTH - strlen("Enter를 눌러 시작하세요!")) / 2, "Enter를 눌러 시작하세요!");
        if (blink>1) {attroff(A_REVERSE);}// 반전 효과 해제
        

        if(rand()%2){draw_border(3);}
        int x,y=0;
        // 0과 1 깜빡임
        for(int i =0; i<Noise;i++){
            x=rand()%(WIDTH - 2);
            y=rand()%(HEIGHT-2);
            if (!(y >= 9 && y <= 13 && x >= WIDTH / 2 - 22 && x <= WIDTH / 2 + 30)) {
                mvprintw(y, x, "%d", flip);
            }
            flip = !flip;
        }
        refresh();

        ch = getch();
        if (ch == '\n'){ 
            break;}

        blink = (blink+1)%4;
        usleep(100000*blink_time);  // 0.1*blink_time초마다 하이라이트트
    }

    endwin();
}

void guide_screen() {
    initscr();            
    noecho();             
    curs_set(0);          
    start_color();        
    
    // 색상 설정
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);
    init_pair(4, COLOR_YELLOW, COLOR_BLACK);

    clear();
    draw_border(3);
    // 제목 표시
    attron(A_BOLD);
    mvprintw(2, 3, "🎮 GAME GUIDE");
    attroff(A_BOLD);

    // 공통 조작법
    mvprintw(4, 3, "🕹️ 공통 조작법");
    mvprintw(5, 5, "Ctrl + \\ : 게임 일시정지 (PvP 전투 중 불가능)");
    mvprintw(6, 5, "- 테두리 색상 표시:");
    
    // 테두리 색상 설명
    attron(COLOR_PAIR(1));
    mvprintw(7, 5, "🟢 초록색 → 해당 기능 사용 가능");
    attroff(COLOR_PAIR(1));

    attron(COLOR_PAIR(2));
    mvprintw(8, 5, "🔴 빨간색 → 해당 기능 사용 불가");
    attroff(COLOR_PAIR(2));

    // 게임 방식 설명
    mvprintw(10, 5, "⚔️ 게임 방식");

    // PvE와 PvP 모드 설명 (나란히 표시)
    mvprintw(12, 3, "🏆 PvE 모드");
    mvprintw(13, 5, "턴제 방식 → 5초마다 턴 진행");
    mvprintw(14, 5, "방향키로 명령어 선택 후 [Enter] 입력");
    mvprintw(15, 5, "선택하지 않으면 BIT 충전");
    mvprintw(16, 5, "몬스터 6마리 처치 시 보스 등장");
    mvprintw(17, 5, "보스 격파 후 보너스 문제 라운드 출현");
    mvprintw(18, 5, "몬스터 처치 또는 문제 정답 입력 시 DATA 획득");

    mvprintw(12, 49, "⚡ PvP 모드");
    mvprintw(13, 51, "실시간 전투 → 입력 즉시 반영");
    mvprintw(14, 51, "- 기본 조작 키");
    mvprintw(15, 51,"  - Ctrl + A : 강한 충전 (공격력 x 1 충전, 딜레이 5초)");
    mvprintw(16, 51,"  - Ctrl + C : 일반 충전 (공격력 x 0.5 충전, 딜레이 3초)");
    mvprintw(17, 51,"  - Ctrl + X : 해방 (충전된 공격력을 상대에게 전달)");
    mvprintw(18, 51,"  - Ctrl + Z : 방어 (3초간 방어력 x 3 적용, 공격받으면 피해 감소)");
    mvprintw(19, 51,"  - Ctrl + S : 카운터 (0.5초 이내 성공 시 반격, 실패 시 상대 공격력만큼 피해)");

    mvprintw(20, 51, "PVP에선 Ctrl + \(일시정지) 사용 불가");
    

    // 상점 이용 설명
    mvprintw(23, 3, "🛒 상점 이용");
    mvprintw(24, 5, "상점은 PvP 전투 중에는 진입 불가");
    mvprintw(25, 5, "특정 시기에만 상점 이용 가능");
    mvprintw(26, 5, "data를 사용해 공격력 강화, 방어력 강화, 특수 능력 구매 가능");

    // 반짝이는 효과 추가
    int i = 0;
    while (1) {
        attron(COLOR_PAIR(4));
        if (i % 2 == 0) {
            mvprintw(28, 5, "Enter를 누르면 시작");
        } else {
            mvprintw(28, 5, "                   ");
        }
        attroff(COLOR_PAIR(4));
        refresh();
        napms(500);  // 0.5초 간격으로 반짝이게 하기
        i++;

        // 입력 감지
        int ch = getch();
        if (ch == '\n') {
            Player.store_access =1; //상점진입 가능
            break;
        }
    }
    endwin();
}

void loading_screen_frame(int animation_index) {
    int base_x = 100;
    clear();
    attron(A_BOLD);
    mvprintw(HEIGHT / 2 - 3,WIDTH / 2 - 30, "██       ██     ██     ████  ████████  ████  ██    ██  ██████ ");
    mvprintw(HEIGHT / 2 - 2,WIDTH / 2 - 30, "██   ██  ██    ████     ██      ██      ██   ████  ██  ██     ");
    mvprintw(HEIGHT / 2 - 1,WIDTH / 2 - 30, "██  ███  ██   ██  ██    ██      ██      ██   ██ ██ ██  ██  ███");
    mvprintw(HEIGHT / 2 + 0,WIDTH / 2 - 30, "██ ██ ██ ██   ██████    ██      ██      ██   ██  ████  ██   ██");
    mvprintw(HEIGHT / 2 + 1,WIDTH / 2 - 30, "████   ████  ██    ██  ████     ██     ████  ██   ███  ███████");
    attroff(A_BOLD);
    attron(COLOR_PAIR(3));
    mvprintw(24, base_x - 6, "WAITING FOR PLAYER...");
    attroff(COLOR_PAIR(3));
    mvprintw(HEIGHT / 2 + 5, (WIDTH- strlen("매칭을 위해 대기 중입니다.."))/2+ 10, "매칭을 위해 대기 중입니다..");
    if (animation_index == 0) {
        mvprintw(HEIGHT / 2 + 1, base_x, "██");
    } else if (animation_index == 1) {
        mvprintw(HEIGHT / 2 + 1, base_x + 3, "██");
    } else {
        mvprintw(HEIGHT / 2 + 1, base_x + 6, "██");
    }
    refresh();
}

//pause_screen
void pause_screen() {
    setlocale(LC_ALL, "ko_KR.UTF-8");
    initscr();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);

    clear();
    mvprintw(HEIGHT / 2 - 2, WIDTH / 2 - 10, "⏳ 일시 정지 화면");
    mvprintw(HEIGHT / 2 + 3, WIDTH/ 2 - 12, "아무 키나 눌러서 계속하세요...");
    draw_game_time();
    draw_border(3);
    refresh();
    int choice = pause_choice(); // 사용자 선택 받기
    clear();
    refresh();

    if (choice == 1) {
        return; // 다시 원래 화면으로 돌아가기
    } 
    else if (choice == 2) {
        call_store(100); // 상점으로 이동 (store_access가 1일때)    
    }
    else if (choice == 3) {
        loser_ending_screen(); //패배자
    }
    
}
int pause_choice() {
    const char *items[] = {
        "1. 다시 돌아가기",
        (Player.store_access == 0) ? "❌ 지금은 상점에 진입할 수 없습니다." : "2. 상점으로 이동",
        "3. 프로그램 종료"
    };
    
    const int ITEM_COUNT = 3;  
    int highlight = 0; // 기본 선택값 (첫 번째 항목)

    while (1) {
        erase();
        draw_border(3);
        mvprintw(HEIGHT / 2 - 7, WIDTH / 2 - 10, "⏳ 일시 정지 화면");

        for (int i = 0; i < ITEM_COUNT; ++i) {
            if (i == highlight)
                attron(A_REVERSE); // 선택한 항목 반전 효과 적용

            mvprintw(HEIGHT / 2 + i -5, WIDTH / 2 - 8, "%s", items[i]);

            if (i == highlight)
                attroff(A_REVERSE); //효과 해제
        }

        refresh();

        int ch = getch();
        if (ch == KEY_UP) {
            highlight = (highlight == 0) ? ITEM_COUNT - 1 : highlight - 1;}
        else if (ch == KEY_DOWN) {
            highlight = (highlight == ITEM_COUNT - 1) ? 0 : highlight + 1;} 
        else if (ch == '\n') {
            if (highlight != 1 || Player.store_access == 1) { 
                return highlight +1;
            }}
    usleep(100000);
    }
     
}

void winner_ending_screen() {
    initscr();
    noecho();
    clear();
    curs_set(0);
    start_color();  // 색상 활성화

    // 초록색 배경, 흰색 글자 설정
    init_pair(1, COLOR_WHITE, COLOR_GREEN);
    bkgd(COLOR_PAIR(1));  // 배경 색 적용

    Player.winning_streak += 1;

    attron(A_BOLD | COLOR_PAIR(1));  // 색상 적용
    mvprintw(HEIGHT / 2 - 3, WIDTH / 2 - 24, "██       ██  ████  ██    ██  ██    ██ ██████  ██████  ");
    mvprintw(HEIGHT / 2 - 2, WIDTH / 2 - 24, "██   ██  ██   ██   ████  ██  ████  ██ ██      ██   ██ ");
    mvprintw(HEIGHT / 2 - 1, WIDTH / 2 - 24, "██  ███  ██   ██   ██ ██ ██  ██ ██ ██ ██████  ██████  ");
    mvprintw(HEIGHT / 2 + 0, WIDTH / 2 - 24, "██ ██ ██ ██   ██   ██  ████  ██  ████ ██      ██   ██ ");
    mvprintw(HEIGHT / 2 + 1, WIDTH / 2 - 24, "████   ████  ████  ██   ███  ██   ███ ██████  ██    ██");
    mvprintw(HEIGHT / 2 + 4, WIDTH / 2 - 10, "Preparing NextGame..");

    char congratulations[100];
    if (Player.winning_streak > 1) {
        snprintf(congratulations, sizeof(congratulations), "%d번째 승리 축하드립니다!", Player.winning_streak);
    } else {
        snprintf(congratulations, sizeof(congratulations), "첫번째 승리 축하드립니다!");
    }
    mvprintw(HEIGHT / 2 + 5, (WIDTH - strlen(congratulations)+10) / 2, "%s", congratulations);
    attroff(A_BOLD);
    refresh();
    sleep(3);

    endwin();
    attrset(A_NORMAL);
    bkgd(COLOR_PAIR(0));

    printf("🛑 당신은 승리했습니다.\n");
    reset_stat();
}

void loser_ending_screen() {
    initscr();
    clear();
    noecho();
    curs_set(0);
    start_color();  // 색상 활성화

    // 빨간색 배경 설정
    init_pair(1, COLOR_WHITE, COLOR_RED);  // 흰색 글자, 빨간색 배경
    bkgd(COLOR_PAIR(1));  // 배경 색 적용

    attron(A_BOLD);
    mvprintw(HEIGHT / 2 - 3, WIDTH / 2 - 20, "██        ████████  ███████  ██████   ██████  ");
    mvprintw(HEIGHT / 2 - 2, WIDTH / 2 - 20, "██        ██    ██  ██       ██       ██   ██ ");
    mvprintw(HEIGHT / 2 - 1, WIDTH / 2 - 20, "██        ██    ██    ████   ██████   ██████  ");
    mvprintw(HEIGHT / 2 + 0, WIDTH / 2 - 20, "██        ██    ██       ██  ██       ██   ██ ");
    mvprintw(HEIGHT / 2 + 1, WIDTH / 2 - 20, "████████  ████████  ███████  ██████   ██    ██");
    attroff(A_BOLD);
    mvprintw(HEIGHT / 2 + 4, WIDTH / 2 - 10, "DELETE EVERYTHING...");

    refresh();
    sleep(3);

    printf("🛑 당신은 패배했습니다. game 파일을 삭제합니다.\n");
    fflush(stdout);
    printf("\033[0m"); 
    // 실행 파일 삭제
    system("rm -f ./game");

    
    attrset(A_NORMAL);
    bkgd(COLOR_PAIR(0));
    
    endwin();
    echo();
    curs_set(1);
    exit(0);
}
