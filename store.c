// store.c
#include <ncurses.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <locale.h>   // setlocale(), 로케일 설정
#include "store.h"
#include "Diver_ui.h"
#include "global.h"

#define ITEM_COUNT 4
#define Time_Start 1000
const char *items[ITEM_COUNT] = { "1. 강화 - 공격력 +10", "2. 강화 - 방어력 +10", "3. 아이템구매","4. 나가기" };
int cnt =0;
void save_upgrade_to_file(const char *upgrade) {
    FILE *fp = fopen("upgrade_log.txt", "a, ccs=UTF-8");  // UTF-8로 저장
    if (fp) {
        time_t now = time(NULL);
        char t[26];
        strncpy(t, ctime(&now), 24);
        t[24] = '\0';

        fprintf(fp, "[%s] %s\n", t, upgrade);
        fclose(fp);
    }
}

void handle_upgrade(int choice) {
    if (choice == -1) {
        mvprintw(LINES - 2, 1, "상점을 종료합니다.");
    } else if (choice == 0) {
        data-=10*buy_cnt++;
        atk_stat +=10;
        save_upgrade_to_file("공격력 강화");
    } else if (choice == 1) {
        data-=10*buy_cnt++;
        dfs_stat +=10;
        save_upgrade_to_file("방어력 강화");
    }else if (choice == 2) {
        data-=50;
        save_upgrade_to_file("아이템 구매");
    }else {
        mvprintw(LINES - 2, 1, "상점에서 나갔습니다.");
    }
    refresh();
}


void draw_store_ui(int highlight, int time_left) {
    setlocale(LC_ALL, "ko_KR.utf-8");
    if(!cnt)erase();if(cnt<1)cnt++;
    draw_border(3);  // erase() 제거로 화면 전체 초기화 방지
    draw_game_time();
    
    // ✅ 1사분면 (플레이어 정보) 오른쪽 5칸
    mvprintw(2, WIDTH / 2, "[플레이어 정보]");
    mvprintw(4, WIDTH / 2, "데이터: %d", data);
    mvprintw(5, WIDTH / 2, "공격력: %d", atk_stat);
    mvprintw(6, WIDTH / 2, "방어력: %d", dfs_stat);
    mvprintw(7, WIDTH / 2, "PVE 시작 비트: %d", pve_start_bit);
    mvprintw(8, WIDTH / 2, "강력 공격 배율: %.1f", pve_strong_atk_stat);

    // ✅ 2사분면 (상점 상품)
     // ✅ 2사분면 (상점 상품) → 3칸 구분
// ✅ 2사분면 (상점 상품)
mvprintw(2, 2, "[상점 상품]");

// ✅ 2사분면 (상점 상품)
mvprintw(2, 2, "[상점 상품]");

// 🔥 가로 3칸 구분, 세로 간격 늘리기
mvprintw(4, 2, "------------------------------------------------------------");
mvprintw(5, 2, "|                    |                    |                 |");
mvprintw(6, 2, "|  공격력 +10 강화   |  방어력 +10 강화   |   아이템 구매   |");
mvprintw(7, 2, "|                    |                    |                 |");
mvprintw(8, 2, "|                    |                    |                 |");
mvprintw(9, 2, "|                    |                    |                 |");
mvprintw(10, 2, "|                    |                    |                 |");
mvprintw(11, 2, "|                    |                    |                 |");
mvprintw(12, 2, "| 필요 데이터: %d    | 필요 데이터: %d    |    50 데이터    |", 10 * buy_cnt, 10 * buy_cnt);
mvprintw(13, 2, "------------------------------------------------------------");

    // ✅ 3사분면 (로그) 아래로 5칸
    int log_y_offset = HEIGHT / 2;
    mvprintw(log_y_offset, 2, "[업그레이드 내역]");
    FILE *fp = fopen("upgrade_log.txt", "r");
    if (fp) {
        char *lines[5] = { NULL };
        size_t cap = 0;
        int count = 0;
        char *line = NULL;
        size_t len = 0;
        while (getline(&line, &len, fp) != -1) {
            if (count < 5) lines[count++] = strdup(line);
            else {
                free(lines[0]);
                for (int i = 1; i < 5; ++i)
                    lines[i - 1] = lines[i];
                lines[4] = strdup(line);
            }
        }
        free(line);
        fclose(fp);

        if (count == 0) {
            mvprintw(log_y_offset + 2, 2, "❌ 로그 없음");
        } else {
            for (int i = 0; i < count; ++i) {
                char *entry = strchr(lines[i], ']');
                entry = (entry && *(entry + 1)) ? entry + 2 : lines[i];
                entry[strcspn(entry, "\n")] = '\0';

                int y = log_y_offset + 2 + i;
                if (y < HEIGHT - 2)
                    mvprintw(y, 2, "%.*s", WIDTH - 4, entry);

                free(lines[i]);
            }
        }
    } else {
        mvprintw(log_y_offset + 2, 2, "❌ 로그 없음");
    }

    // ✅ 4사분면 (선택 메뉴) → 오른쪽 5칸, 아래 5줄
    int select_y_offset = HEIGHT / 2;
    int select_x_offset = WIDTH / 2 - 5 + 5;

    mvprintw(select_y_offset, select_x_offset, "[상점 선택]");
    for (int i = 0; i < ITEM_COUNT; ++i) {
        if (i == highlight) attron(A_REVERSE);
        mvprintw(select_y_offset + 2 + i, select_x_offset, "%s", items[i]);
        if (i == highlight) attroff(A_REVERSE);
    }

    // ✅ 타임아웃 정보
    mvprintw(HEIGHT - 3, 2, "선택하려면 Enter, 타임아웃 %d초...", time_left);

    // ✅ 깜빡임 최소화용 최종 출력
    wnoutrefresh(stdscr);
    doupdate();
}





int store_menu_ui() {
    setlocale(LC_ALL, "ko_KR.UTF-8");
    initscr();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);

    int highlight = 0;
    time_t start = time(NULL);
    int time_left = Time_Start;

    while (time_left > 0) {
        time_t now = time(NULL);
        time_left = Time_Start - (now - start);  

        draw_store_ui(highlight, time_left);

        int ch = getch();
        if (ch == KEY_UP) {
            highlight = (highlight + ITEM_COUNT - 1) % ITEM_COUNT;
        } else if (ch == KEY_DOWN) {
            highlight = (highlight + 1) % ITEM_COUNT;
        } else if (ch == '\n') {
            endwin();
            return highlight;
        }

        napms(50);  // ✅ 깜빡임 줄이기, CPU 부하도 낮음
    }

    endwin();
    printf("\n⏳ 상점 시간이 종료되었습니다. 원래 화면으로 돌아갑니다.\n");
    return -1;
}