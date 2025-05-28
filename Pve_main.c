//합치는 과정에서 문제 ############################################으로 기록록
#include "Pve_game.h"
#include "Diver_ui.h"
#include "global.h"
#include "store.h"
#include "log.h"
//#include "log.h"//log_file ############################

int Player_DATA_BAR_WIDTH=200;
int monster_DATA_BAR_WIDTH=50;
int boss_DATA_BAR_WIDTH=200;

// 전역 변수 정의
Entity player = {200, 5, 100, 100, 20};//data, bit, attack, strong_attack, defense
Entity monster = {50, 5, 10, 50, 10};
Entity boss = {200,5,20,100,20};
//Entity boss = {150, 5, 20, 100,20};

int main() {
    setlocale(LC_ALL, "");
    initscr();
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);    // 딜레이 시 (붉은 테두리)
    init_pair(2, COLOR_GREEN, COLOR_BLACK);  // 딜레이 시 (초록 테두리)

    //srand(time(NULL));

    //int bonus_rand=rand() % 10;

    
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(FALSE);
    //srand(time(NULL));

    attron(COLOR_PAIR(2));
    box(stdscr, 0, 0);
    attroff(COLOR_PAIR(2));
    refresh();

    int turn = 0;
    int round = 1;
    int selected_action = 0;
    char player_action_result[100] = "";
    char monster_action_result[100] = "";

    draw_game_time();//###############################################
    int boss_count = 0;

    int cure_data=0;

    srand(time(NULL));
    int monster_No=rand()%3;
    int bonus_rand=rand() % 10;

    while (player.data > 0 && monster.data > 0) {
        print_status(turn, 2, round, player, monster, selected_action);
        draw_ui(player, monster, round,monster_No);
        //draw_game_time();//########################################

        mvprintw(33, 10, "플레이어 액션: %s", player_action_result);//log_file로 바꾸기
        mvprintw(34, 10, "몬스터   액션: %s", monster_action_result);
        //write_log_file(player_action_result, "Pve_log.txt");###########################
        //write_log_file(monster_action_result, "Pve_log.txt");#######################
        
        //read_log_file("Pve_log.txt", 32, 10);#######################



        refresh();
        
        int acted = wait_for_input_with_timeout(&selected_action, 3, turn, round, player, monster, player_action_result, monster_action_result,monster_No);
        draw_game_time();//########################################

        if (!acted) selected_action = 3;

        switch (selected_action) {
            case 0:
                if (player.bit >= 1) {
                    monster.data -= player.attack;
                    player.bit--;
                    snprintf(player_action_result, 100, "공격!");
                } else {
                    snprintf(player_action_result, 100, "BIT 부족!");
                    attron(COLOR_PAIR(1));
                    box(stdscr, 0, 0);
                    attroff(COLOR_PAIR(1));
                }
                break;
            case 1:
                if (player.bit >= 5) {
                    monster.data -= player.strong_attack;
                    player.bit -= 5;
                    snprintf(player_action_result, 100, "강화 공격!");
                } else {
                    snprintf(player_action_result, 100, "BIT 부족!");
                    attron(COLOR_PAIR(1));
                    box(stdscr, 0, 0);
                    attroff(COLOR_PAIR(1));
                }
                break;
            case 2:
                snprintf(player_action_result, 100, "방어!");
                break;
            case 3:
                if (player.bit < BIT_BAR_WIDTH) {
                    player.bit++;
                    snprintf(player_action_result, 100, "BIT 충전!");
                } else snprintf(player_action_result, 100, "BIT가 꽉 찼습니다!");
                break;
            case 4:
                if (player.bit >= 1) {
                    player.bit--;
                    snprintf(player_action_result, 100, "회피!");
                } else {
                    snprintf(player_action_result, 100, "BIT 부족!");
                    attron(COLOR_PAIR(1));
                    box(stdscr, 0, 0);
                    attroff(COLOR_PAIR(1));
                }
                break;
        }

        monster_turn(&monster, &player, monster_action_result, round, turn, selected_action,monster_No);

        turn++;

        if (monster.data <= 0) {

            //srand(time(NULL));
            monster_No=rand()%3;
            cure_data+=(Player_DATA_BAR_WIDTH - player.data);
            if (round % 7 == 0) { // 보스 라운드라면
                bonus_round(boss_count + bonus_rand, &player);
                boss_count++;   
                boss.attack+=30;
                boss.strong_attack=boss.attack*5;
                boss.defense+=30;

                monster.attack+=10;
                monster.strong_attack=boss.attack*5;
                monster.defense+=10;
            }

            round++;
            if(round % 7 ==0){
                monster.data=boss_DATA_BAR_WIDTH;
                monster.attack=boss.attack;
                monster.strong_attack=boss.strong_attack;
                monster.defense=boss.defense;
            }else{
                monster.data=monster_DATA_BAR_WIDTH;
                monster.attack=monster.attack;
                monster.strong_attack=monster.strong_attack;
                monster.defense=monster.defense;
            }
            monster.bit = 5;
            turn = 0;

            erase();

            attron(COLOR_PAIR(2));
            box(stdscr, 0, 0);
            attroff(COLOR_PAIR(2));

            mvprintw(TERM_HEIGHT / 2 - 8, (TERM_WIDTH - 30) / 2, "전투 후 부상 data 일부 획득 !!  +%d data",cure_data/2);
            //mvprintw(33, 120, "몬스터 타입: %d", monster_No);

            player.data+=(cure_data/2);
            if (player.data > Player_DATA_BAR_WIDTH) Player_DATA_BAR_WIDTH=player.data;

            if(round % 7 == 0){
                mvprintw(TERM_HEIGHT / 2 - 7, (TERM_WIDTH - 30) / 2, "승리 보상 data 획득 !!         +100 data");
                mvprintw(TERM_HEIGHT / 2, (TERM_WIDTH - 30) / 2, "🔥위험!! ** 보스 출현 **🔥");
                player.data+=100;
                if (player.data > Player_DATA_BAR_WIDTH) Player_DATA_BAR_WIDTH=player.data;
                
            } else {
                mvprintw(TERM_HEIGHT / 2 - 7, (TERM_WIDTH - 30) / 2, "승리 보상 data 획득 !!         +30 data");
                mvprintw(TERM_HEIGHT / 2, (TERM_WIDTH - 30) / 2, "새로운 적이 등장했습니다!");
                player.data+=30;
                if (player.data > Player_DATA_BAR_WIDTH) Player_DATA_BAR_WIDTH=player.data;
            }
            
            if(player.data < Player_DATA_BAR_WIDTH) Player_DATA_BAR_WIDTH=player.data;//Player_DATA_BAR_WIDTH도 줄어들수 있다 @@@@@@

            mvprintw(TERM_HEIGHT / 2 + 4, (TERM_WIDTH - 30) / 2, "계속하려면 Enter를 누르세요...");
            refresh();

            cure_data=0;

            // 사용자로부터 Enter 키를 기다림
            int ch;
            while ((ch = getch()) != '\n' && ch != KEY_ENTER) {
            // 아무 동작 없음, 엔터가 아닐 경우 다시 기다림
            }
            //napms(1500);
        }

        //if(병조 출력하는 전체실행시간 > 12분) -> is(start_time >= 720)storecall()함수 호출####################################################
        //###################게임 도중에 storecall들어가면 storecall내에서 DTA_BAR_WITH와 player.data 감소시켜야 할것 같다. 
    }

    erase();
    attron(COLOR_PAIR(2));
    box(stdscr, 0, 0);
    attroff(COLOR_PAIR(2));
    
    if (player.data <= 0 && monster.data <= 0)
        mvprintw(TERM_HEIGHT / 2, (TERM_WIDTH - 30) / 2, "무승부!");
    else if (player.data <= 0)
        mvprintw(TERM_HEIGHT / 2, (TERM_WIDTH - 30) / 2, "Monster wins!");
    else if (monster.data <= 0)
        mvprintw(TERM_HEIGHT / 2, (TERM_WIDTH - 30) / 2, "Player wins!");

    mvprintw(TERM_HEIGHT / 2 + 2, (TERM_WIDTH - 40) / 2, "Press ENTER to exit the game...");
    refresh();
    while (getch() != '\n');

    endwin();
    return 0;
}