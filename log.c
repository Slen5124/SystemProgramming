#include <ncurses.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <locale.h>   
#include "store.h"
#include "Diver_ui.h"
#include "global.h"
// 병조파트
// upgrade는 쓸 내용, 로그파일 네임은 저장할 파일
void write_log_file( char *upgrade, const char *logfilename) {
    FILE *fp = fopen(logfilename, "a, ccs=UTF-8");  // UTF-8로 저장
    if (fp) {
        time_t now = time(NULL);
        char t[26];
        strncpy(t, ctime(&now), 24);
        t[24] = '\0';

        // 형식: [시간] <닉네임> 메시지
        fprintf(fp, "[%s] <%s> %s\n", t, Player.nick, upgrade);
        fclose(fp);
    }
}


// upgrade는 쓸 내용, 로그파일 네임은 저장할 , nick 맞춰서 출력
void read_log_file(const char *logfilename, int y_offset, int x_offset) {
    FILE *fp = fopen(logfilename, "r");
    if (fp) {
        char *lines[5] = { NULL };
        int count = 0;
        char *line = NULL;
        size_t len = 0;

       
        char nicktag[128];
        snprintf(nicktag, sizeof(nicktag), "<%s>",Player.nick);

        while (getline(&line, &len, fp) != -1) {
            if (strstr(line, nicktag)) {
                if (count < 5) {
                    lines[count++] = strdup(line);
                } else {
                    free(lines[0]);
                    for (int i = 1; i < 5; ++i)
                        lines[i - 1] = lines[i];
                    lines[4] = strdup(line);
                }
            }
        }
        free(line);
        fclose(fp);

        if (count == 0) {
            mvprintw(y_offset + 2, x_offset, "❌ %s님의 로그 없음", Player.nick);
        } else {
            // 출력 전 영역 지우기
            for (int i = 0; i < 5; ++i) {
                int y = y_offset + 2 + i;
                if (y < HEIGHT - 2) {
                    mvprintw(y, x_offset, "%-*s", WIDTH - 4, " ");
                }
            }

            // 로그 출력
            for (int i = 0; i < count; ++i) {
                char *entry = strchr(lines[i], '>');  // <nick> 다음부터 메시지
                entry = (entry && *(entry + 1)) ? entry + 2 : lines[i];
                entry[strcspn(entry, "\n")] = '\0';

                int y = y_offset + 2 + i;
                if (y < HEIGHT - 2) {
                    mvprintw(y, x_offset, "%.*s", WIDTH - 4, entry);
                }

                free(lines[i]);
            }
        }
    } else {
        mvprintw(y_offset + 2, x_offset, "❌ 로그 파일 열기 실패");
    }
}