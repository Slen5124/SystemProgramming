    #include "Pve_game.h"
    #include "Diver_ui.h"
    #include "global.h"
    #include "store.h"
    #include <unistd.h>

    extern int ROUND_MON_NO;
    extern MonsterInfo monsters[];
    extern MonsterInfo current_monster;
    extern PlayerState Player;
    #define PVE_TIME 300


    void monster_turn(MonsterInfo *monster, PlayerState *Player, char *monster_action_result, int round, int turn, int selected_action, int monster_No) {
        // 몬스터별 행동 패턴 정의
        // 0: 공격, 1: 강화 공격, 2: 방어, 3: 충전, 4: 회피
        int pattern_didos[] = {0,0,3,2,3};      // 디도스      충전-공격-충전-방어
        int pattern_lootkit[]  = {3,1,3,3,0,3,0,3,3};        // 루키트       충전-공격-방어
        int pattern_gpt[]     = {3,0,2};      // 지피티티        충전-공격-충전-강화공격
        int pattern_boss[]   = {3,1,3,3,0,3,0,3,3,2}; // 보스           충전-강화공격-방어-충전-회피-공격-강화공격

        int *pattern;
        int pattern_length;

        if(round % 7 == 0){
            pattern = pattern_boss;
            pattern_length = sizeof(pattern_boss) / sizeof(int);
            monster_No=3;
        } else {
            switch(monster_No){
            case 0:
                pattern = pattern_didos;
                pattern_length = sizeof(pattern_didos) / sizeof(int);
                break;
            case 1:
                pattern = pattern_lootkit;
                pattern_length = sizeof(pattern_lootkit) / sizeof(int);
                break;
            case 2:
                pattern = pattern_gpt;
                pattern_length = sizeof(pattern_gpt) / sizeof(int);
                break;
            }
        
        }

        int action_index = pattern[turn % pattern_length];

        // 행동 실행
        switch (action_index) {
            case 0: // 공격
                if (monsters[monster_No].bit >= 1) {
                    if (selected_action == 2) { // 플레이어 방어일때
                        if(Player->dfs_stat < monsters[monster_No].attack)
                            Player->data -= (monsters[monster_No].attack - Player->dfs_stat);
                    } else if(selected_action != 4) { // 플레이어 회피아닐때
                        Player->data -= monsters[monster_No].attack;
                    }
                    monsters[monster_No].bit--;
                    snprintf(monster_action_result, 100, "공격!");
                } else {
                    snprintf(monster_action_result, 100, "BIT 부족!");
                }
                break;
            case 1: // 강화 공격
                if (monsters[monster_No].bit >= 4) {
                    if (selected_action == 2) { // 플레이어 방어일때
                        if(Player->dfs_stat < monsters[monster_No].strong_attack)
                            Player->data -= (monsters[monster_No].strong_attack - Player->dfs_stat);
                    } else if(selected_action != 4) { // 플레이어 회피아닐때
                        Player->data -= monsters[monster_No].strong_attack;
                    }
                    monsters[monster_No].bit -= 4;
                    snprintf(monster_action_result, 100, "강화 공격!");
                } else {
                    snprintf(monster_action_result, 100, "BIT 부족!");
                }
                break;
            case 2: // 방어
                if(selected_action == 0 ){
                    if(monsters[monster_No].defense < Player -> atk_stat)
                        monsters[monster_No].data += monsters[monster_No].defense;
                    else
                        monsters[monster_No].data+=Player->atk_stat;
                }else if(selected_action == 1){
                    if(monsters[monster_No].defense < (Player -> atk_stat * Player->pve_strong_atk_stat))
                        monsters[monster_No].data += monsters[monster_No].defense;
                    else
                        monsters[monster_No].data+=Player -> atk_stat * Player->pve_strong_atk_stat;
                }
                snprintf(monster_action_result, 100, "방어!");
                break;
            case 3: // BIT 충전
                if (monsters[monster_No].bit < BIT_BAR_WIDTH) {
                    monsters[monster_No].bit++;
                    snprintf(monster_action_result, 100, "BIT 충전!");
                } else {
                    snprintf(monster_action_result, 100, "BIT가 꽉 찼습니다!");
                }
                break;
            case 4: // 회피
                if(selected_action == 0){ 
                    monsters[monster_No].data += Player->atk_stat;
                } else if(selected_action == 1){
                    monsters[monster_No].data += Player -> atk_stat * Player->pve_strong_atk_stat;
                }
                monsters[monster_No].bit--;
                snprintf(monster_action_result, 100, "회피!");
                break;
            default:
                snprintf(monster_action_result, 100, "이상 행동");
        }
    }

    int wait_for_input_with_timeout(int *selected_action, int timeout_sec, int turn, int round, PlayerState Player, MonsterInfo monster, char *player_action_result, char *monster_action_result,int monster_No) {
        time_t start_time = time(NULL);
        int ch;
        int action_confirmed = 0;

        while (1) {
            int elapsed = (int)(time(NULL) - start_time);
            int remaining = timeout_sec - elapsed;
            if (remaining <= 0) {
                if(action_confirmed == 1){
                    return 1;
                } else {
                    return 0;
                }
            }
            timeout(100);
            ch = getch();
            if (ch != ERR) {
                if (!action_confirmed) {
                    switch (ch) {
                        case KEY_UP:
                            *selected_action = (*selected_action + 4) % 5;
                            break;
                        case KEY_DOWN:
                            *selected_action = (*selected_action + 1) % 5;
                            break;
                        case '\n':
                            action_confirmed = 1;
                            break;
                        case 'q':
                            endwin();
                            exit(0);
                    }
                } else {
                    if (ch == 'q') {
                        endwin();
                        exit(0);
                    }
                }
            }

            erase();
            
            
            print_status(turn, remaining, round, Player, monster, *selected_action);
            draw_ui(Player, monster, round,monster_No);
            draw_game_time();

            mvprintw(30, 10, "플레이어 액션: %s", player_action_result);
            mvprintw(31, 10, "몬스터   액션: %s", monster_action_result);
            draw_border(1);
            refresh();
        }
    }

    void bonus_round(int boss_count, PlayerState *Player) {
        clear();

        // 입력모드 초기화
        nocbreak();
        noraw();
        cbreak();
        echo();
        curs_set(1);

        // 문제와 정답 배열
        char *questions[] = {
            "사용자 프로그램이 운영체제에 도움을 요청할 때 사용하는 함수를 뭐라고 하나요?",
            "하나의 컴퓨터를 여러 사용자가 동시에 사용하는 것처럼 보이게 하는 기술은?",
            "파일 디스크립터를 조작하는데 사용하는 시스템 호출 함수는?",
            "create()함수를 open()으로 대체하려고 할때 들어가야 할 플래그는?"
        };

        char *answers[] = {
            "시스템콜",
            "time sharing",
            "fcntl",
            "O_CREAT|O_WRONLY|O_TRUNC"
        };

        int total_questions = sizeof(questions) / sizeof(questions[0]);
        int idx = boss_count % total_questions;

        char input[50] = {0};

        int row, col;
        getmaxyx(stdscr, row, col);

    const char* monster_ascii[] = {
        "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀",
        "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣀⣄⣀⢀⢤⢤⡤⡤⣄⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀",
        "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡤⣞⣯⢿⣺⣗⣯⣫⣗⡯⣟⣗⣯⢯⢦⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀",
        "⠀⠀⠀⠀⠀⠀⠀⠀⡠⡮⡯⣗⣿⢽⣟⣞⣗⢷⣳⢯⡷⡯⣞⡿⡽⣞⣦⠀⠀⠀⠀⠀⠀⠀⠀",
        "⠀⠀⠀⠀⠀⠀⠀⣲⡳⣝⢽⢵⣻⢝⠎⡂⡪⡙⡺⣝⢽⣽⣫⢯⣟⣗⣯⢇⠀⠀⠀⠀⠀⠀⠀",
        "⠀⠀⠀⠀⠀⠀⠀⣗⣽⣺⡽⣯⢳⢑⢁⢂⢐⠨⠐⠜⢜⢺⢺⢯⡷⣯⣯⢿⡅⠀⠀⠀⠀⠀⠀",
        "⠀⠀⠀⠀⠀⠀⠀⣷⣻⣞⣟⢎⢕⢄⢢⠠⠐⠀⠅⠡⡡⡊⡌⡎⣿⣳⣯⢿⡕⠀⠀⠀⠀⠀⠀",
        "⠀⠀⠀⠀⠀⠀⠀⠓⣗⡗⠡⡑⠄⡂⠄⡊⢌⠐⠨⡨⠐⠄⠌⠔⡁⡓⣯⡟⠀⠀⠀⠀⠀⠀⠀",
        "⠀⠀⠀⠀⠀⠀⠀⠀⡜⡌⠨⠐⠍⡊⡫⠘⡄⠅⢕⡘⡘⡫⢙⠐⢌⢊⢞⠄⠀⠀⠀⠀⠀⠀⠀",
        "⠀⠀⠀⠀⠀⠀⠀⠀⢊⠆⡈⠄⡁⡐⡀⠅⡊⡀⡂⢆⢐⠀⢂⢁⠂⡂⡇⠁⠀⠀⠀⠀⠀⠀⠀",
        "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠐⡀⢂⠠⠀⠄⢊⠤⢄⢢⢂⠅⡐⠀⠄⢂⠂⠅⠀⠀⠀⠀⠀⠀⠀⠀",
        "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠂⡂⢂⠂⠌⠄⢌⡐⣐⢐⠨⠀⠅⠌⡂⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀",
        "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠐⢐⢐⠨⠨⠩⡑⡅⡣⡹⢈⠊⠌⠢⡈⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀",
        "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡐⢔⠡⠡⡁⠢⢈⢐⠐⠄⢕⠡⣃⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀",
        "⠀⠀⠀⠀⠀⠀⠀⠀⠀⡠⣪⠃⡂⠕⡑⢔⢡⢢⢢⢡⠱⠡⡑⠄⣿⣢⡀⠀⠀⠀⠀⠀⠀⠀⠀",
        "⠀⠀⠀⠀⠀⠀⣀⢤⡺⣝⣽⠀⠀⠁⡊⠔⡐⠅⡂⠢⠡⢑⠀⠀⣳⣗⣿⣲⢦⣤⣀⢀⠀⠀⠀",
        "⢀⣀⢤⡲⡼⣝⡮⣗⡯⣗⣯⠂⠀⠀⠀⠈⠄⠅⠌⠌⠈⠀⠀⠀⣗⣟⡾⣽⣻⢾⢽⣽⡽⣶⡤",
        "⢸⣺⢽⣺⢯⣗⡯⣗⡯⣗⣟⡆⠀⢀⢤⣖⢾⢔⣖⡶⣤⢄⠀⠀⡷⣯⣟⡷⣯⣟⣿⣺⡽⣗⡯",
        "⢸⢯⡯⣯⢟⡾⣽⣳⣻⢽⣺⡆⠄⠀⠀⠫⣯⣻⣪⡏⠁⠀⠁⠂⣿⣳⢷⣻⣗⣿⣺⣳⢿⢽⢯",
        "⢸⡯⣯⡯⡿⣽⣳⣻⢞⣯⢷⢇⠀⠀⠀⢀⢹⢞⡾⡀⠀⠄⠀⢅⡿⣞⣯⣷⣻⣞⣷⣻⣽⣻⣗",
        "⢸⡯⣷⣟⣯⢷⣻⣞⣯⢷⣻⢧⢀⠀⠀⠀⣾⡫⡯⣇⠀⠀⢀⢺⡽⣯⢷⣗⣿⣺⣞⣷⣻⣞⣮",
        "⢸⣽⣳⣻⣞⣯⢷⣻⣺⢯⡯⣿⠀⠀⠀⢸⡵⡯⡯⣷⡀⠂⢐⣯⢿⣽⣻⣞⣷⣻⣞⣷⣻⣺⣳",
        "⢸⣞⣷⣻⢾⡽⡯⣷⣻⣽⢽⡽⡎⠀⠀⣗⡯⣯⡻⣮⡣⠀⢸⣺⣟⣾⣳⣻⣞⣾⣳⢷⢯⡷⣗"
        };

        int monster_ascii_lines = sizeof(monster_ascii) / sizeof(monster_ascii[0]);

        for (int i = 0; i < monster_ascii_lines; i++) {
            if (row / 2 - monster_ascii_lines / 2 + i >= 0 && row / 2 - monster_ascii_lines / 2 + i < row) {
                mvprintw(row / 2 - monster_ascii_lines / 2 + i, (col - strlen(monster_ascii[0])) / 2, "%s", monster_ascii[i]);
            }
        }

        // 보너스 텍스트
        const char *bonus_text = "✨✨✨ *** 보너스 문제 *** ✨✨✨";
        attron(COLOR_PAIR(1) | A_BOLD);
        mvprintw(row / 2 - 2, (col - strlen(bonus_text)) / 2 + 7, "%s", bonus_text);
        attroff(COLOR_PAIR(1) | A_BOLD);

        // 문제 출력
        attron(COLOR_PAIR(2));
        mvprintw(row / 2, (col - strlen(questions[idx])) / 2 + 40, "%s", questions[idx]);
        attroff(COLOR_PAIR(2));

        // 입력 칸 표시 및 위치 설정
        int input_y = row / 2;
        int input_x = (col - 30) / 2;
        mvprintw(input_y+3, input_x, "입력: ");
        move(input_y+3, input_x + 7);

        refresh();

        timeout(-1);
        getnstr(input, 49);

        noecho();
        curs_set(0);
        refresh();    // 지운 내용을 실제 터미널에 반영

        // 결과 출력
        if (strcmp(input, answers[idx]) == 0) {
            mvprintw(row / 2 + 6, (col - 20) / 2, "정답입니다! DATA + 50");
            Player->data += 50;
            if (Player->data > Player_DATA_BAR_WIDTH) Player_DATA_BAR_WIDTH=Player->data;
        } 
        else if(strcmp(input, "I'm mhan") == 0){
            mvprintw(row / 2 +5, (col - 20) / 2,"💕💕💕💕💕💕I love mhan💕💕💕💕💕💕");       
            mvprintw(row / 2 + 6, (col - 20) / 2, "무조건!!! 정답입니다! DATA + 1000");
            Player->data += 1000;
            if (Player->data > Player_DATA_BAR_WIDTH) Player_DATA_BAR_WIDTH=Player->data;
        }
        else {
            mvprintw(row / 2 + 6, (col - 20) / 2, "틀렸습니다...");
        }

        mvprintw(row / 2 + 8, (col - 20) / 2, "계속하려면 엔터를 누르세요...");
        refresh();
        while (getch() != '\n');  // 엔터 입력 대기
    } 


    // 플레이어 행동 처리 함수
    void handle_player_action(int selected_action, char* player_action_result,int monster_No) {
        switch (selected_action) {
            case 0: // 일반 공격
                if (Player.bit >= 1) {
                    monsters[monster_No].data -= Player.atk_stat;
                    Player.bit --;
                    snprintf(player_action_result, 100, "공격!");
                } else {
                    snprintf(player_action_result, 100, "BIT 부족!");
                    draw_border(2);
                }
                break;
                
            case 1: // 강화 공격
                if (Player.bit >= 4) {
                    monsters[monster_No].data -= Player.atk_stat * Player.pve_strong_atk_stat;
                    Player.bit -= 4;
                    snprintf(player_action_result, 100, "강화 공격!");
                } else {
                    snprintf(player_action_result, 100, "BIT 부족!");
                    draw_border(2);
                }
                break;
                
            case 2: // 방어
                snprintf(player_action_result, 100, "방어!");
                break;
                
            case 3: // BIT 충전
                if (Player.bit < BIT_BAR_WIDTH) {
                    Player.bit++;
                    snprintf(player_action_result, 100, "BIT 충전!");
                } else {
                    snprintf(player_action_result, 100, "BIT가 꽉 찼습니다!");
                }
                break;
                
            case 4: // 회피
                if (Player.bit >= 1) {
                    Player.bit--;
                    snprintf(player_action_result, 100, "회피!");
                } else {
                    snprintf(player_action_result, 100, "BIT 부족!");
                draw_border(2);
                }
                break;
        }
    }

    // 라운드 종료 처리 함수
    void handle_round_end(int* round, int* turn, int* cure_data, int* boss_count, int bonus_rand, int* monster_No) {
        srand(time(NULL));
        *monster_No = rand() % 3;
        *cure_data += (Player_DATA_BAR_WIDTH - Player.data);
        Player.bit=Player.pve_start_bit;

        monsters[*monster_No].data=monsters[*monster_No].max_data;
        monsters[*monster_No].bit=5;
        
        if (*round % 7 == 0) { // 보스 라운드
        
            monsters[3].attack=(int)(1.5*monsters[3].attack);
            monsters[3].strong_attack = monsters[3].attack*5;
            monsters[3].defense=(int)(1.5*monsters[3].defense);
            monsters[3].max_data=(int)(1.5*monsters[3].max_data);
            monsters[3].data=(int)(1.5*monsters[3].data);

            for(int i=0;i<3;i++){
                monsters[i].attack=(int)(1.5*monsters[i].attack);
                monsters[i].strong_attack = monsters[i].attack*5;
                monsters[i].defense=(int)(1.5*monsters[i].defense);
                monsters[i].max_data=(int)(1.5*monsters[i].max_data);
                monsters[i].data=(int)(1.5*monsters[i].data);

            }
        }

        ROUND_MON_NO ++;
        *turn = 0;
    }


    // 메인 게임 루프 함수
    void game_loop(){
        int turn = 0;
        int round = 1;
        int selected_action = 0;
        char player_action_result[100] = "";
        char monster_action_result[100] = "";
        int boss_count = 0;
        int cure_data = 0;
        Player.bit=Player.pve_start_bit;
        
        srand(time(NULL));
        int monster_No = rand() % 3;
        int bonus_rand = rand() % 10;
        
        draw_game_time();
        //current_monster = monsters[monster_No];
        
        
        while (Player.data > 0 && monsters[monster_No].data > 0) {
            if (time(NULL) - Player.start_time >= PVE_TIME) {
                    call_store(60);
                    break;
            } // 5분 지나면 상점으로 호출.. 병조 코드와 병합 되는 부분분

            print_status(turn, 2, round, Player, monsters[monster_No], selected_action);
            draw_ui(Player, monsters[monster_No], round, monster_No);
            
            mvprintw(33, 10, "플레이어 액션: %s", player_action_result);
            mvprintw(34, 10, "몬스터   액션: %s", monster_action_result);
            
            refresh();
            
            int acted = wait_for_input_with_timeout(&selected_action, 3, turn, round, Player, monsters[monster_No], player_action_result, monster_action_result, monster_No);
            draw_game_time();
            
            if (!acted) selected_action = 3;
            
            handle_player_action(selected_action, player_action_result,monster_No);
            monster_turn(&monsters[monster_No], &Player, monster_action_result, round, turn, selected_action, monster_No);
            
            turn++;
            if(Player.data <= 0){loser_ending_screen();}
            if (monsters[monster_No].data <= 0) {
                handle_round_end(&round, &turn, &cure_data, &boss_count,bonus_rand, &monster_No);
                display_victory_screen(round, cure_data);

                if(round % 7 == 0){
                    bonus_round(boss_count + bonus_rand, &Player);
                    (boss_count)++;
                }
                round++;


                if(round % 7 == 0){
                    monster_No=3;
                }
                cure_data = 0;
            
            }
        } 
    }